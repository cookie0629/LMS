
#pragma once

#include <string_view>

#include "SubsonicResponse.hpp"

namespace lms::db
{
    class Cluster;
}

namespace lms::api::subsonic
{
    Response::Node createItemGenreNode(std::string_view name);
}
