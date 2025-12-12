
#pragma once

#include <Wt/Http/Request.h>
#include <Wt/Http/Response.h>

#include "SubsonicResponse.hpp"

namespace lms::api::subsonic
{
    struct RequestContext;

    Response handleGetLyrics(RequestContext& context);
    Response handleGetLyricsBySongId(RequestContext& context);

    void handleDownload(RequestContext& context, const Wt::Http::Request& request, Wt::Http::Response& response);
    void handleStream(RequestContext& context, const Wt::Http::Request& request, Wt::Http::Response& response);
    void handleGetCoverArt(RequestContext& context, const Wt::Http::Request& request, Wt::Http::Response& response);
} // namespace lms::api::subsonic
