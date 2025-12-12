
#pragma once

#include "database/Object.hpp"

#include "SubsonicResponse.hpp"

namespace lms::db
{
    class Track;
    class User;
    class Session;
} // namespace lms::db

namespace lms::api::subsonic
{
    struct RequestContext;

    Response::Node createSongNode(RequestContext& context, const db::ObjectPtr<db::Track>& track, bool id3);
} // namespace lms::api::subsonic