
#pragma once

#include "RequestContext.hpp"
#include "SubsonicResponse.hpp"

namespace lms::api::subsonic
{
    Response handlePingRequest(RequestContext& context);
    Response handleGetLicenseRequest(RequestContext& context);
    Response handleGetOpenSubsonicExtensions(RequestContext& context);
} // namespace lms::api::subsonic
