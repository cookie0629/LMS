
#pragma once

#include "database/Object.hpp"

#include "SubsonicResponse.hpp"

namespace lms::db
{
    class Cluster;
}

namespace lms::api::subsonic
{
    struct RequestContext;

    Response::Node createGenreNode(RequestContext& context, const db::ObjectPtr<db::Cluster>& cluster);
} // namespace lms::api::subsonic
