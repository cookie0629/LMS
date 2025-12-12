
#pragma once

#include "RequestContext.hpp"
#include "SubsonicResponse.hpp"

namespace lms::api::subsonic::Scan
{
    Response handleGetScanStatus(RequestContext& context);
    Response handleStartScan(RequestContext& context);
} // namespace lms::api::subsonic::Scan
