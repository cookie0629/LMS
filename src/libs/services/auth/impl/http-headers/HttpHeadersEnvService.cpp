
#include "HttpHeadersEnvService.hpp"

#include <Wt/WEnvironment.h>

#include "core/IConfig.hpp"
#include "core/ILogger.hpp"
#include "core/Service.hpp"

namespace lms::auth
{
    HttpHeadersEnvService::HttpHeadersEnvService(db::IDb& db)
        : AuthServiceBase{ db }
        , _fieldName{ core::Service<core::IConfig>::get()->getString("http-headers-login-field", "X-Forwarded-User") }
    {
        LMS_LOG(AUTH, INFO, "Using http header field = '" << _fieldName << "'");
    }

    HttpHeadersEnvService::CheckResult HttpHeadersEnvService::processEnv(const Wt::WEnvironment& env)
    {
        const std::string loginName{ env.headerValue(_fieldName) };
        if (loginName.empty())
            return CheckResult{ .state = CheckResult::State::Denied, .userId = {} };

        LMS_LOG(AUTH, DEBUG, "Extracted login name = '" << loginName << "' from HTTP header");

        const db::UserId userId{ getOrCreateUser(loginName) };
        onUserAuthenticated(userId);
        return CheckResult{ .state = CheckResult::State::Granted, .userId = userId };
    }

    HttpHeadersEnvService::CheckResult HttpHeadersEnvService::processRequest(const Wt::Http::Request& request)
    {
        const std::string loginName{ request.headerValue(_fieldName) };
        if (loginName.empty())
            return CheckResult{ .state = CheckResult::State::Denied, .userId = {} };

        LMS_LOG(AUTH, DEBUG, "Extracted login name = '" << loginName << "' from HTTP header");

        const db::UserId userId{ getOrCreateUser(loginName) };
        onUserAuthenticated(userId);
        return { .state = CheckResult::State::Granted, .userId = userId };
    }
} // namespace lms::auth
