
#pragma once

#include <optional>

#include "DatabaseCollectorBase.hpp"
#include "database/Types.hpp"
#include "database/objects/ArtistId.hpp"

namespace lms::db
{
    class Artist;
}

namespace lms::ui
{
    class ArtistCollector : public DatabaseCollectorBase
    {
    public:
        using DatabaseCollectorBase::DatabaseCollectorBase;

        db::RangeResults<db::ArtistId> get(std::optional<db::Range> range = std::nullopt);
        void reset() { _randomArtists.reset(); }
        void setArtistLinkType(std::optional<db::TrackArtistLinkType> linkType) { _linkType = linkType; }

    private:
        db::RangeResults<db::ArtistId> getRandomArtists(Range range);
        std::optional<db::RangeResults<db::ArtistId>> _randomArtists;
        std::optional<db::TrackArtistLinkType> _linkType;
    };
} // namespace lms::ui
