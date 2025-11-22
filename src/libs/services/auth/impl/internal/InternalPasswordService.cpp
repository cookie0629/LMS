#include "InternalPasswordService.hpp"

#include <Wt/WRandom.h>

#include "core/IConfig.hpp"
#include "core/Service.hpp"
#include "core/ILogger.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/User.hpp"

namespace lms::auth
{
    InternalPasswordService::InternalPasswordService(db::IDb& db, std::size_t maxThrottlerEntries)
        : PasswordServiceBase{ db, maxThrottlerEntries }
        , _bcryptRoundCount{ 12 }  // 默认 bcrypt 轮数，可以从配置读取
        , _hashFunc{ static_cast<int>(_bcryptRoundCount) }
    {
        // 可以从配置读取 bcrypt 轮数
        if (auto* config = lms::core::Service<lms::core::IConfig>::get())
        {
            // 暂时使用默认值，后续可以从配置读取
            // _bcryptRoundCount = static_cast<unsigned>(config->getULong("internal-password-bcrypt-round", 12));
        }

        if (_bcryptRoundCount < 7 || _bcryptRoundCount > 31)
        {
            throw Exception{ "bcrypt round count must be in range 7-31" };
        }
    }

    bool InternalPasswordService::checkUserPassword(std::string_view loginName, std::string_view password)
    {
        auto& session = getDbSession();
        auto transaction = session.createReadTransaction();

        auto user = db::User::find(session, loginName);
        if (!user)
        {
            return false;
        }

        auto passwordHash = user->getPasswordHash();
        if (passwordHash.salt.empty() || passwordHash.hash.empty())
        {
            return false;
        }

        // 验证密码
        bool passwordMatched = _hashFunc.verify(
            std::string{ password },
            std::string{ passwordHash.salt },
            std::string{ passwordHash.hash });

        // 如果密码匹配但 bcrypt 轮数不同，更新密码哈希
        if (passwordMatched && passwordHash.bcryptRoundCount != _bcryptRoundCount)
        {
            // 需要写事务来更新密码哈希
            auto writeTransaction = session.createWriteTransaction();
            const auto updatedPasswordHash = hashPassword(password);
            user.modify()->setPasswordHash(updatedPasswordHash);
        }

        return passwordMatched;
    }

    bool InternalPasswordService::canSetPasswords() const
    {
        return true;
    }

    IPasswordService::PasswordAcceptabilityResult InternalPasswordService::checkPasswordAcceptability(
        std::string_view password,
        const PasswordValidationContext& context) const
    {
        // 检查密码强度
        auto strength = _validator.evaluateStrength(std::string{ password }, context.loginName, "");
        if (!strength.isValid())
        {
            return PasswordAcceptabilityResult::TooWeak;
        }

        // 检查密码是否与登录名相同
        if (password == context.loginName)
        {
            return PasswordAcceptabilityResult::MustMatchLoginName;
        }

        return PasswordAcceptabilityResult::OK;
    }

    void InternalPasswordService::setPassword(db::UserId userId, std::string_view newPassword)
    {
        auto& session = getDbSession();
        auto transaction = session.createWriteTransaction();

        auto user = db::User::find(session, userId);
        if (!user)
        {
            throw UserNotFoundException{};
        }

        const auto passwordHash = hashPassword(newPassword);
        user.modify()->setPasswordHash(passwordHash);
    }

    db::User::PasswordHash InternalPasswordService::hashPassword(std::string_view password) const
    {
        // 生成随机盐（使用 Wt::WRandom）
        const std::string salt{ Wt::WRandom::generateId(32) };

        return db::User::PasswordHash{
            .bcryptRoundCount = _bcryptRoundCount,
            .salt = salt,
            .hash = _hashFunc.compute(std::string{ password }, salt)
        };
    }
} // namespace lms::auth

