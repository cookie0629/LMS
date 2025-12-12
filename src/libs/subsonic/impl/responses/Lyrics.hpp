
#pragma once

#include "database/Object.hpp"

#include "SubsonicResponse.hpp"

namespace lms::db
{
    class TrackLyrics;
}

namespace lms::api::subsonic
{
    struct RequestContext;

    Response::Node createLyricsNode(RequestContext& context, const db::ObjectPtr<db::TrackLyrics>& lyrics);
    Response::Node createStructuredLyricsNode(RequestContext& context, const db::ObjectPtr<db::TrackLyrics>& lyrics);
} // namespace lms::api::subsonic
