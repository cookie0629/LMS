
#pragma once

#include "database/Object.hpp"

#include "SubsonicResponse.hpp"

namespace lms::db
{
    class Label;
}

namespace lms::api::subsonic
{
    Response::Node createRecordLabel(const db::ObjectPtr<db::Label>& label);
}