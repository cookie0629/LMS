
#pragma once

#include <memory>

#include "core/EnumSet.hpp"
#include "database/Types.hpp"
#include "database/objects/TrackListId.hpp"
#include "services/recommendation/Types.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::recommendation
{
    class IEngine
    {
    public:
        virtual ~IEngine() = default;

        virtual void load(bool forceReload, const ProgressCallback& progressCallback = {}) = 0;
        virtual void requestCancelLoad() = 0;

        virtual TrackContainer findSimilarTracksFromTrackList(db::TrackListId tracklistId, std::size_t maxCount) const = 0;
        virtual TrackContainer findSimilarTracks(const std::vector<db::TrackId>& tracksId, std::size_t maxCount) const = 0;
        virtual ReleaseContainer getSimilarReleases(db::ReleaseId releaseId, std::size_t maxCount) const = 0;
        virtual ArtistContainer getSimilarArtists(db::ArtistId artistId, core::EnumSet<db::TrackArtistLinkType> linkTypes, std::size_t maxCount) const = 0;
    };

    std::unique_ptr<IEngine> createEngine(db::IDb& db);

} // namespace lms::recommendation
