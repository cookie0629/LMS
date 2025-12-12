
#pragma once

#include <optional>
#include <string>

#include "core/UUID.hpp"

namespace lms::scanner
{
    // See:
    // - for the content for the info file: https://kodi.wiki/view/NFO_files/Artists
    // - for the definition of some mb fields: https://musicbrainz.org/doc/Artist
    struct ArtistInfo
    {
        std::string name;
        std::optional<core::UUID> mbid;
        std::string sortName;       // mb
        std::string type;           // mb
        std::string gender;         // mb
        std::string disambiguation; // mb
        std::string biography;
    };
} // namespace lms::scanner
