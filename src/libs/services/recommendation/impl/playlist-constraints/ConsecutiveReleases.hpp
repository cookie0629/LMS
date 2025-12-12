
#pragma once

#include "IConstraint.hpp"

#include "database/objects/ReleaseId.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::recommendation::PlaylistGeneratorConstraint
{
    class ConsecutiveReleases : public IConstraint
    {
    public:
        ConsecutiveReleases(db::IDb& db);
        ~ConsecutiveReleases() override = default;
        ConsecutiveReleases(const ConsecutiveReleases&) = delete;
        ConsecutiveReleases& operator=(const ConsecutiveReleases&) = delete;

    private:
        float computeScore(const std::vector<db::TrackId>& trackIds, std::size_t trackIndex) override;

        db::ReleaseId getReleaseId(db::TrackId trackId);

        db::IDb& _db;
    };
} // namespace lms::recommendation::PlaylistGeneratorConstraint
