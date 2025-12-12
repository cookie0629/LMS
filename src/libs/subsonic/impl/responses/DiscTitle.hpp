
#pragma once

#include "database/Object.hpp"

#include "SubsonicResponse.hpp"

namespace lms::db
{
    class Medium;
}

namespace lms::api::subsonic
{
    Response::Node createDiscTitle(const db::ObjectPtr<db::Medium>& medium);
}