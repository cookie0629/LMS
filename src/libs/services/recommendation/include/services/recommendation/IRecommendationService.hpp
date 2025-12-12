
#pragma once

#include <memory>
#include <vector>

#include "core/EnumSet.hpp"
#include "database/Types.hpp"
#include "database/objects/ArtistId.hpp"
#include "database/objects/ReleaseId.hpp"
#include "database/objects/TrackListId.hpp"
#include "services/recommendation/Types.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::recommendation
{
    class IRecommendationService
    {
    public:
        virtual ~IRecommendationService() = default;

        virtual void load() = 0;

        virtual TrackContainer findSimilarTracks(db::TrackListId tracklistId, std::size_t maxCount) const = 0;
        virtual TrackContainer findSimilarTracks(const std::vector<db::TrackId>& tracksId, std::size_t maxCount) const = 0;
        virtual ReleaseContainer getSimilarReleases(db::ReleaseId releaseId, std::size_t maxCount) const = 0;
        virtual ArtistContainer getSimilarArtists(db::ArtistId artistId, core::EnumSet<db::TrackArtistLinkType> linkTypes, std::size_t maxCount) const = 0;
    };

    std::unique_ptr<IRecommendationService> createRecommendationService(db::IDb& db);
} // namespace lms::recommendation
