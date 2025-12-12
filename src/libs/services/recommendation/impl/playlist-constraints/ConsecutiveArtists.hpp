
#pragma once

#include "IConstraint.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::recommendation::PlaylistGeneratorConstraint
{
    class ConsecutiveArtists : public IConstraint
    {
    public:
        ConsecutiveArtists(db::IDb& db);
        ~ConsecutiveArtists() override = default;
        ConsecutiveArtists(const ConsecutiveArtists&) = delete;
        ConsecutiveArtists& operator=(const ConsecutiveArtists&) = delete;

    private:
        float computeScore(const TrackContainer& trackIds, std::size_t trackIndex) override;
        ArtistContainer getArtists(db::TrackId trackId);

        db::IDb& _db;
    };
} // namespace lms::recommendation::PlaylistGeneratorConstraint
