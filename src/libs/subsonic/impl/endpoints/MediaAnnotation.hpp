
#pragma once

#include "RequestContext.hpp"
#include "SubsonicResponse.hpp"

namespace lms::api::subsonic
{
    Response handleStarRequest(RequestContext& context);
    Response handleUnstarRequest(RequestContext& context);
    Response handleSetRating(RequestContext& context);
    Response handleScrobble(RequestContext& context);
} // namespace lms::api::subsonic