
#pragma once

#include "core/PartialDateTime.hpp"

#include "SubsonicResponse.hpp"

namespace lms::api::subsonic
{
    Response::Node createItemDateNode(const core::PartialDateTime& date);
}
