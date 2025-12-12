
#pragma once

#include <memory>
#include <string_view>

#include "database/objects/UserId.hpp"

namespace lms::db
{
    class IDb;
    class Session;
} // namespace lms::db

namespace Wt
{
    class WEnvironment;
}

namespace Wt::Http
{
    class Request;
}

namespace lms::auth
{
    class IEnvService
    {
    public:
        virtual ~IEnvService() = default;

        // Auth Token services
        struct CheckResult
        {
            enum class State
            {
                Granted,
                Denied,
                Throttled,
            };

            State state{ State::Denied };
            db::UserId userId{};
        };

        virtual CheckResult processEnv(const Wt::WEnvironment& env) = 0;
        virtual CheckResult processRequest(const Wt::Http::Request& request) = 0;
    };

    std::unique_ptr<IEnvService> createEnvService(std::string_view backend, db::IDb& db);
} // namespace lms::auth
