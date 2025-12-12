
#pragma once

#include "database/Object.hpp"

#include "SubsonicResponse.hpp"

namespace lms::db
{
    class Medium;
    class Track;
} // namespace lms::db

namespace lms::api::subsonic
{
    Response::Node createReplayGainNode(const db::ObjectPtr<db::Track>& track, const db::ObjectPtr<db::Medium>& medium);
}