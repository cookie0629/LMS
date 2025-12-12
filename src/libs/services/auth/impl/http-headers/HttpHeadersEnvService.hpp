
#pragma once

#include "services/auth/IEnvService.hpp"

#include "AuthServiceBase.hpp"

namespace lms::auth
{
    class HttpHeadersEnvService : public IEnvService, public AuthServiceBase
    {
    public:
        HttpHeadersEnvService(db::IDb& db);

    private:
        CheckResult processEnv(const Wt::WEnvironment& env) override;
        CheckResult processRequest(const Wt::Http::Request& request) override;

        std::string _fieldName;
    };

} // namespace lms::auth
