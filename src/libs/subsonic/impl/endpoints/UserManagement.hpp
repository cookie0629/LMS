
#pragma once

#include "RequestContext.hpp"
#include "SubsonicResponse.hpp"

namespace lms::api::subsonic
{
    Response handleGetUserRequest(RequestContext& context);
    Response handleGetUsersRequest(RequestContext& context);
    Response handleCreateUserRequest(RequestContext& context);
    Response handleUpdateUserRequest(RequestContext& context);
    Response handleDeleteUserRequest(RequestContext& context);
    Response handleChangePassword(RequestContext& context);
} // namespace lms::api::subsonic
