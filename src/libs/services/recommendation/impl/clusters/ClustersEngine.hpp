
#pragma once

#include "IEngine.hpp"

namespace lms::recommendation
{

    class ClusterEngine : public IEngine
    {
    public:
        ClusterEngine(db::IDb& db)
            : _db{ db } {}

        ~ClusterEngine() override = default;
        ClusterEngine(const ClusterEngine&) = delete;
        ClusterEngine(ClusterEngine&&) = delete;
        ClusterEngine& operator=(const ClusterEngine&) = delete;
        ClusterEngine& operator=(ClusterEngine&&) = delete;

    private:
        void load(bool /*forceReload*/, const ProgressCallback& /*progressCallback*/) override {}
        void requestCancelLoad() override {}

        TrackContainer findSimilarTracksFromTrackList(db::TrackListId tracklistId, std::size_t maxCount) const override;
        TrackContainer findSimilarTracks(const std::vector<db::TrackId>& trackIds, std::size_t maxCount) const override;
        ReleaseContainer getSimilarReleases(db::ReleaseId releaseId, std::size_t maxCount) const override;
        ArtistContainer getSimilarArtists(db::ArtistId artistId, core::EnumSet<db::TrackArtistLinkType> linkTypes, std::size_t maxCount) const override;

        db::IDb& _db;
    };

} // namespace lms::recommendation
