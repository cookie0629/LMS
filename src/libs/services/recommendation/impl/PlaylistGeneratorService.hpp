
#pragma once

#include "services/recommendation/IPlaylistGeneratorService.hpp"
#include "services/recommendation/IRecommendationService.hpp"

#include "playlist-constraints/IConstraint.hpp"

namespace lms::recommendation
{
    class PlaylistGeneratorService : public IPlaylistGeneratorService
    {
    public:
        PlaylistGeneratorService(db::IDb& db, IRecommendationService& recommendationService);

    private:
        TrackContainer extendPlaylist(db::TrackListId tracklistId, std::size_t maxCount) const override;

        TrackContainer getTracksFromTrackList(db::TrackListId tracklistId) const;

        db::IDb& _db;
        IRecommendationService& _recommendationService;
        std::vector<std::unique_ptr<PlaylistGeneratorConstraint::IConstraint>> _constraints;
    };
} // namespace lms::recommendation
