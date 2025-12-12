
#include "services/auth/IEnvService.hpp"

#include "services/auth/Types.hpp"

#include "http-headers/HttpHeadersEnvService.hpp"

namespace lms::auth
{
    std::unique_ptr<IEnvService> createEnvService(std::string_view backendName, db::IDb& db)
    {
        if (backendName == "http-headers")
            return std::make_unique<HttpHeadersEnvService>(db);

        throw Exception{ "Authentication backend '" + std::string{ backendName } + "' is not supported!" };
    }
} // namespace lms::auth
