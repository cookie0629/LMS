
#pragma once

#include "database/Object.hpp"

#include "SubsonicResponse.hpp"

namespace lms::db
{
    class Directory;
    class Release;
    class User;
    class Session;
} // namespace lms::db

namespace lms::api::subsonic
{
    struct RequestContext;

    Response::Node createAlbumNode(RequestContext& context, const db::ObjectPtr<db::Release>& release, bool id3, const db::ObjectPtr<db::Directory>& directory = {});
} // namespace lms::api::subsonic