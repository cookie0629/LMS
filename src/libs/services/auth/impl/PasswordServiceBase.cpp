
#include "PasswordServiceBase.hpp"

#include <Wt/Auth/HashFunction.h>
#include <Wt/WRandom.h>

#include "internal/InternalPasswordService.hpp"
#ifdef LMS_SUPPORT_PAM
    #include "pam/PAMPasswordService.hpp"
#endif // LMS_SUPPORT_PAM

#include "core/ILogger.hpp"
#include "database/Session.hpp"
#include "services/auth/Types.hpp"

namespace lms::auth
{
    static const Wt::Auth::SHA1HashFunction sha1Function;

    std::unique_ptr<IPasswordService> createPasswordService(std::string_view backend, db::IDb& db, std::size_t maxThrottlerEntryCount)
    {
        if (backend == "internal")
            return std::make_unique<InternalPasswordService>(db, maxThrottlerEntryCount);
#ifdef LMS_SUPPORT_PAM
        if (backend == "PAM")
            return std::make_unique<PAMPasswordService>(db, maxThrottlerEntryCount);
#endif // LMS_SUPPORT_PAM
        throw Exception{ "Authentication backend '" + std::string{ backend } + "' not supported!" };
    }

    PasswordServiceBase::PasswordServiceBase(db::IDb& db, std::size_t maxThrottlerEntries)
        : AuthServiceBase{ db }
        , _loginThrottler{ maxThrottlerEntries }
    {
    }

    PasswordServiceBase::CheckResult PasswordServiceBase::checkUserPassword(const boost::asio::ip::address& clientAddress, std::string_view loginName, std::string_view password)
    {
        LMS_LOG(AUTH, DEBUG, "Checking password for user '" << loginName << "'");

        // Do not waste too much resource on brute force attacks (optim)
        {
            std::shared_lock lock{ _mutex };

            if (_loginThrottler.isClientThrottled(clientAddress))
                return CheckResult{ .state = CheckResult::State::Throttled, .userId = {} };
        }

        const bool match{ checkUserPassword(loginName, password) };
        {
            std::unique_lock lock{ _mutex };

            if (_loginThrottler.isClientThrottled(clientAddress))
                return CheckResult{ .state = CheckResult::State::Throttled, .userId = {} };

            if (match)
            {
                _loginThrottler.onGoodClientAttempt(clientAddress);

                const db::UserId userId{ getOrCreateUser(loginName) };
                onUserAuthenticated(userId);
                return CheckResult{ .state = CheckResult::State::Granted, .userId = userId };
            }

            _loginThrottler.onBadClientAttempt(clientAddress);
            return CheckResult{ .state = CheckResult::State::Denied, .userId = {} };
        }
    }
} // namespace lms::auth
