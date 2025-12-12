
#pragma once

#include <string>
#include <vector>

#include "database/Object.hpp"
#include "database/Types.hpp"

#include "SubsonicResponse.hpp"

namespace lms::db
{
    class Artist;
    class User;
    class Session;
} // namespace lms::db

namespace lms::api::subsonic
{
    struct RequestContext;

    namespace utils
    {
        std::string joinArtistNames(const std::vector<db::ObjectPtr<db::Artist>>& artists);
        std::string_view toString(db::TrackArtistLinkType type);
    } // namespace utils

    Response::Node createArtistNode(RequestContext& context, const db::ObjectPtr<db::Artist>& artist);
    Response::Node createArtistNode(const db::ObjectPtr<db::Artist>& artist); // only minimal info
} // namespace lms::api::subsonic