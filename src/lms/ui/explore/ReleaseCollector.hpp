
#pragma once

#include <optional>

#include "DatabaseCollectorBase.hpp"

#include "database/Types.hpp"
#include "database/objects/ReleaseId.hpp"

namespace lms::db
{
    class Release;
}

namespace lms::ui
{
    class ReleaseCollector : public DatabaseCollectorBase
    {
    public:
        using DatabaseCollectorBase::DatabaseCollectorBase;

        db::RangeResults<db::ReleaseId> get(std::optional<db::Range> range = std::nullopt);
        void reset() { _randomReleases.reset(); }

    private:
        db::RangeResults<db::ReleaseId> getRandomReleases(Range range);
        std::optional<db::RangeResults<db::ReleaseId>> _randomReleases;
    };
} // namespace lms::ui
