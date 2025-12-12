
#pragma once

#include "database/Object.hpp"

#include "SubsonicResponse.hpp"

namespace lms::db
{
    class User;
}

namespace lms::api::subsonic
{
    struct RequestContext;

    Response::Node createUserNode(RequestContext& context, const db::ObjectPtr<db::User>& user);
} // namespace lms::api::subsonic