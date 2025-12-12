
#pragma once

#include "RequestContext.hpp"
#include "SubsonicResponse.hpp"

namespace lms::api::subsonic
{
    Response handleSearch2Request(RequestContext& context);
    Response handleSearch3Request(RequestContext& context);
} // namespace lms::api::subsonic
