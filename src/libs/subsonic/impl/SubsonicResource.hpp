#pragma once

#include <Wt/Http/Request.h>
#include <Wt/Http/Response.h>
#include <Wt/WResource.h>

#include "database/objects/UserId.hpp"

#include "SubsonicResourceConfig.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::api::subsonic
{
    class RequestContext;

    class SubsonicResource final : public Wt::WResource
    {
    public:
        SubsonicResource(db::IDb& db);

    private:
        void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response) override;

        using MediaRetrievalHandlerFunc = std::function<void(RequestContext&, const Wt::Http::Request&, Wt::Http::Response&)>;
        void handleMediaRetrievalRequest(MediaRetrievalHandlerFunc handler, const Wt::Http::Request& request, Wt::Http::Response& response);

        db::UserId authenticateUser(const Wt::Http::Request& request);

        const SubsonicResourceConfig _config;
        db::IDb& _db;
    };
} // namespace lms::api::subsonic
