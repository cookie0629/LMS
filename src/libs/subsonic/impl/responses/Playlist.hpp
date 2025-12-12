
#pragma once

#include "database/Object.hpp"

#include "SubsonicResponse.hpp"

namespace lms::db
{
    class TrackList;
} // namespace lms::db

namespace lms::api::subsonic
{
    struct RequestContext;

    Response::Node createPlaylistNode(RequestContext& context, const db::ObjectPtr<db::TrackList>& tracklist);
} // namespace lms::api::subsonic