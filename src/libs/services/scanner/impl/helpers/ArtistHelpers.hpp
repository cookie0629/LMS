
#pragma once

#include "core/TaggedType.hpp"

#include "database/objects/Artist.hpp"

namespace lms::scanner
{
    struct Artist;
}

namespace lms::scanner::helpers
{
    using AllowFallbackOnMBIDEntry = core::TaggedBool<struct AllowFallbackOnMBIDEntryTag>;

    db::Artist::pointer getOrCreateArtistByMBID(db::Session& session, const Artist& artistInfo, AllowFallbackOnMBIDEntry allowFallbackOnMBIDEntries);
    db::Artist::pointer getOrCreateArtistByName(db::Session& session, const Artist& artistInfo, AllowFallbackOnMBIDEntry allowFallbackOnMBIDEntries);
    db::Artist::pointer getOrCreateArtist(db::Session& session, const Artist& artistInfo, AllowFallbackOnMBIDEntry allowFallbackOnMBIDEntries);

} // namespace lms::scanner::helpers