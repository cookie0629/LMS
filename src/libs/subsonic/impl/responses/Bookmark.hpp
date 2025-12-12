
#pragma once

#include "database/Object.hpp"

#include "SubsonicResponse.hpp"

namespace lms::db
{
    class TrackBookmark;
}

namespace lms::api::subsonic
{
    Response::Node createBookmarkNode(const db::ObjectPtr<db::TrackBookmark>& bookmark);
}