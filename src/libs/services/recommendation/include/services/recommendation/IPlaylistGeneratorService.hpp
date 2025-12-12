
#pragma once

#include <memory>

#include "database/objects/TrackListId.hpp"
#include "services/recommendation/Types.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::recommendation
{
    class IRecommendationService;
    class IPlaylistGeneratorService
    {
    public:
        virtual ~IPlaylistGeneratorService() = default;

        // extend an existing playlist with similar tracks (but use playlist contraints)
        virtual TrackContainer extendPlaylist(db::TrackListId tracklistId, std::size_t maxCount) const = 0;
    };

    std::unique_ptr<IPlaylistGeneratorService> createPlaylistGeneratorService(db::IDb& db, IRecommendationService& recommendationService);
} // namespace lms::recommendation
