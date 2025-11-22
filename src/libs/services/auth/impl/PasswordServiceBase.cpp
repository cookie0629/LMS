#include "PasswordServiceBase.hpp"

#include <boost/asio/ip/address.hpp>

namespace lms::auth
{
    PasswordServiceBase::PasswordServiceBase(db::IDb& db, std::size_t maxThrottlerEntries)
        : AuthServiceBase{ db }
        , _loginThrottler{ maxThrottlerEntries }
    {
    }

    IPasswordService::CheckResult PasswordServiceBase::checkUserPassword(
        const boost::asio::ip::address& clientAddress,
        std::string_view loginName,
        std::string_view password)
    {
        std::unique_lock lock(_mutex);

        CheckResult result;

        // 检查是否被限流
        if (_loginThrottler.isClientThrottled(clientAddress))
        {
            result.state = CheckResult::State::Throttled;
            return result;
        }

        // 检查密码
        bool passwordValid = checkUserPassword(loginName, password);

        if (passwordValid)
        {
            // 密码正确
            _loginThrottler.onGoodClientAttempt(clientAddress);
            
            // 获取用户ID
            auto& session = getDbSession();
            auto transaction = session.createReadTransaction();
            auto user = db::User::find(session, loginName);
            
            if (user)
            {
                result.state = CheckResult::State::Granted;
                result.userId = user->getId();
                onUserAuthenticated(result.userId);
            }
            else
            {
                result.state = CheckResult::State::Denied;
            }
        }
        else
        {
            // 密码错误
            _loginThrottler.onBadClientAttempt(clientAddress);
            result.state = CheckResult::State::Denied;
        }

        return result;
    }
} // namespace lms::auth

