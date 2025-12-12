
#pragma once

#include "database/Object.hpp"

#include "SubsonicResponse.hpp"

namespace lms::db
{
    class Release;
} // namespace lms::db

namespace lms::api::subsonic
{
    struct RequestContext;

    Response::Node createAlbumInfoNode(RequestContext& context, const db::ObjectPtr<db::Release>& release);
} // namespace lms::api::subsonic