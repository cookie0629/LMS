#pragma once

#include <Wt/Auth/HashFunction.h>
#include <Wt/Auth/PasswordStrengthValidator.h>

#include "database/objects/User.hpp"
#include "PasswordServiceBase.hpp"
#include "services/auth/IPasswordService.hpp"

namespace lms::auth
{
    /**
     * @brief 内部密码服务
     * 使用数据库存储密码哈希
     */
    class InternalPasswordService : public PasswordServiceBase
    {
    public:
        InternalPasswordService(db::IDb& db, std::size_t maxThrottlerEntries);

    private:
        bool checkUserPassword(std::string_view loginName, std::string_view password) override;

        bool canSetPasswords() const override;
        PasswordAcceptabilityResult checkPasswordAcceptability(
            std::string_view password,
            const PasswordValidationContext& context) const override;
        void setPassword(db::UserId userId, std::string_view newPassword) override;

        /**
         * @brief 哈希密码
         * @param password 明文密码
         * @return 密码哈希
         */
        db::User::PasswordHash hashPassword(std::string_view password) const;

        const unsigned _bcryptRoundCount;
        const Wt::Auth::BCryptHashFunction _hashFunc;
        Wt::Auth::PasswordStrengthValidator _validator;
    };
} // namespace lms::auth

