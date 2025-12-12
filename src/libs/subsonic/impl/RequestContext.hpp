
#pragma once

#include <iosfwd>
#include <string>
#include <string_view>

#include <Wt/Http/Request.h>

#include "database/Object.hpp"

#include "ProtocolVersion.hpp"
#include "ResponseFormat.hpp"
#include "SubsonicResourceConfig.hpp"

namespace lms::db
{
    class Session;
    class User;
} // namespace lms::db

namespace lms::api::subsonic
{
    class RequestContext
    {
    public:
        RequestContext(const Wt::Http::Request& request, db::Session& dbSession, db::ObjectPtr<db::User> user, const SubsonicResourceConfig& config);
        ~RequestContext();
        RequestContext(const RequestContext&) = delete;
        RequestContext& operator=(const RequestContext&) = delete;

        using ParameterMap = Wt::Http::ParameterMap;

        const ParameterMap& getParameters() const;
        std::istream& getBody() const;

        db::Session& getDbSession();
        db::ObjectPtr<db::User> getUser() const;

        std::string getClientIpAddr() const;
        std::string_view getClientName() const;

        ProtocolVersion getServerProtocolVersion() const;
        ResponseFormat getResponseFormat() const;
        bool isOpenSubsonicEnabled() const;

    private:
        const Wt::Http::Request& _request;
        db::Session& _dbSession;
        db::ObjectPtr<db::User> _user;
        const SubsonicResourceConfig& _config;

        const std::string _clientName;
        const ProtocolVersion _clientProtocolVersion;
        const ResponseFormat _responseFormat;

        const ProtocolVersion _serverProtocolVersion;
        const bool _isOpenSubsonicEnabled;
    };

} // namespace lms::api::subsonic
