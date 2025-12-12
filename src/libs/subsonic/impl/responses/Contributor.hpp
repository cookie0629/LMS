
#pragma once

#include "database/Object.hpp"

#include "SubsonicResponse.hpp"

namespace lms::db
{
    class Artist;
    class TrackArtistLink;
} // namespace lms::db

namespace lms::api::subsonic
{
    Response::Node createContributorNode(const db::ObjectPtr<db::TrackArtistLink>& trackArtistLink, const db::ObjectPtr<db::Artist>& artist);
}