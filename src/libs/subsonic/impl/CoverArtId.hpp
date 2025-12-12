
#pragma once

#include <ctime>

#include "core/String.hpp"
#include "database/objects/ArtworkId.hpp"

namespace lms::api::subsonic
{
    struct CoverArtId
    {
        db::ArtworkId id;
        std::time_t timestamp;
    };

    std::string idToString(CoverArtId coverId);
} // namespace lms::api::subsonic

// Used to parse parameters
namespace lms::core::stringUtils
{
    template<>
    std::optional<api::subsonic::CoverArtId> readAs(std::string_view str);
} // namespace lms::core::stringUtils
