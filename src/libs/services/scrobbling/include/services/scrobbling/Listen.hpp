
#pragma once

#include <Wt/WDateTime.h>

#include "database/objects/TrackId.hpp"
#include "database/objects/UserId.hpp"

namespace lms::scrobbling
{
    struct Listen
    {
        db::UserId userId{};
        db::TrackId trackId{};
    };

    struct TimedListen : public Listen
    {
        Wt::WDateTime listenedAt;
    };
} // namespace lms::scrobbling
