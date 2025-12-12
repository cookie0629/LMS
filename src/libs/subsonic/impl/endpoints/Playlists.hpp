
#pragma once

#include "RequestContext.hpp"
#include "SubsonicResponse.hpp"

namespace lms::api::subsonic
{
    Response handleGetPlaylistsRequest(RequestContext& context);
    Response handleGetPlaylistRequest(RequestContext& context);
    Response handleCreatePlaylistRequest(RequestContext& context);
    Response handleUpdatePlaylistRequest(RequestContext& context);
    Response handleDeletePlaylistRequest(RequestContext& context);
} // namespace lms::api::subsonic