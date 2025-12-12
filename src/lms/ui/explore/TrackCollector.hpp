
#pragma once

#include <optional>

#include "database/objects/TrackId.hpp"

#include "DatabaseCollectorBase.hpp"

namespace lms::db
{
    class Track;
}

namespace lms::ui
{
    class TrackCollector : public DatabaseCollectorBase
    {
    public:
        using DatabaseCollectorBase::DatabaseCollectorBase;

        db::RangeResults<db::TrackId> get(std::optional<db::Range> range = std::nullopt);
        void reset() { _randomTracks.reset(); }

    private:
        db::RangeResults<db::TrackId> getRandomTracks(Range range);
        std::optional<db::RangeResults<db::TrackId>> _randomTracks;
    };
} // namespace lms::ui
