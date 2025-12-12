
#pragma once

#include "RequestContext.hpp"
#include "SubsonicResponse.hpp"

namespace lms::api::subsonic
{
    Response handleGetBookmarks(RequestContext& context);
    Response handleCreateBookmark(RequestContext& context);
    Response handleDeleteBookmark(RequestContext& context);
    Response handleGetPlayQueue(RequestContext& context);
    Response handleSavePlayQueue(RequestContext& context);
} // namespace lms::api::subsonic
