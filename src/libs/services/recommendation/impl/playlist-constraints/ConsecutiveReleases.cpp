
#include "ConsecutiveReleases.hpp"

#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/objects/Release.hpp"
#include "database/objects/Track.hpp"

namespace lms::recommendation::PlaylistGeneratorConstraint
{
    ConsecutiveReleases::ConsecutiveReleases(db::IDb& db)
        : _db{ db }
    {
    }

    float ConsecutiveReleases::computeScore(const std::vector<db::TrackId>& trackIds, std::size_t trackIndex)
    {
        assert(!trackIds.empty());
        assert(trackIndex <= trackIds.size() - 1);

        const db::ReleaseId releaseId{ getReleaseId(trackIds[trackIndex]) };

        constexpr std::size_t rangeSize{ 3 }; // check up to rangeSize tracks before/after the target track
        static_assert(rangeSize > 0);

        float score{};
        for (std::size_t i{ 1 }; i < rangeSize; ++i)
        {
            if ((trackIndex >= i) && getReleaseId(trackIds[trackIndex - i]) == releaseId)
                score += (1.F / static_cast<float>(i));

            if ((trackIndex + i < trackIds.size()) && getReleaseId(trackIds[trackIndex + i]) == releaseId)
                score += (1.F / static_cast<float>(i));
        }

        return score;
    }

    db::ReleaseId ConsecutiveReleases::getReleaseId(db::TrackId trackId)
    {
        using namespace db;

        Session& dbSession{ _db.getTLSSession() };
        auto transaction{ dbSession.createReadTransaction() };

        const Track::pointer track{ Track::find(dbSession, trackId) };
        if (!track)
            return {};

        const Release::pointer release{ track->getRelease() };
        if (!release)
            return {};

        return release->getId();
    }
} // namespace lms::recommendation::PlaylistGeneratorConstraint
