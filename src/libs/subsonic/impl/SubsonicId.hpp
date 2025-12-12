
#pragma once

#include "core/String.hpp"
#include "database/objects/ArtistId.hpp"
#include "database/objects/DirectoryId.hpp"
#include "database/objects/MediaLibraryId.hpp"
#include "database/objects/PodcastEpisodeId.hpp"
#include "database/objects/PodcastId.hpp"
#include "database/objects/ReleaseId.hpp"
#include "database/objects/TrackId.hpp"
#include "database/objects/TrackListId.hpp"

namespace lms::api::subsonic
{
    std::string idToString(db::ArtistId id);
    std::string idToString(db::DirectoryId id);
    std::string idToString(db::PodcastEpisodeId id);
    std::string idToString(db::PodcastId id);
    std::string idToString(db::ReleaseId id);
    std::string idToString(db::TrackId id);
    std::string idToString(db::TrackListId id);
} // namespace lms::api::subsonic

// Used to parse parameters
namespace lms::core::stringUtils
{
    template<>
    std::optional<db::ArtistId> readAs(std::string_view str);

    template<>
    std::optional<db::DirectoryId> readAs(std::string_view str);

    template<>
    std::optional<db::MediaLibraryId> readAs(std::string_view str);

    template<>
    std::optional<db::PodcastEpisodeId> readAs(std::string_view str);

    template<>
    std::optional<db::PodcastId> readAs(std::string_view str);

    template<>
    std::optional<db::ReleaseId> readAs(std::string_view str);

    template<>
    std::optional<db::TrackId> readAs(std::string_view str);

    template<>
    std::optional<db::TrackListId> readAs(std::string_view str);
} // namespace lms::core::stringUtils
