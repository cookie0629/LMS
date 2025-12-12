

#pragma once

#include <optional>
#include <string_view>

#include <Wt/WDate.h>

#include "types/TrackMetadata.hpp"

namespace lms::scanner::utils
{
    Wt::WDate parseDate(std::string_view dateStr);
    std::optional<int> parseYear(std::string_view yearStr);

    struct PerformerArtist
    {
        Artist artist;
        std::string role;
    };

    // format is "artist name (role)"
    PerformerArtist extractPerformerAndRole(std::string_view entry);
} // namespace lms::scanner::utils
