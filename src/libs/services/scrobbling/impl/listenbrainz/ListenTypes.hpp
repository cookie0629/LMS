
#pragma once

#include <ostream>
#include <string>

#include <Wt/WDateTime.h>

#include "core/UUID.hpp"

namespace lms::scrobbling::listenBrainz
{
    struct Listen
    {
        std::string trackName;
        std::string releaseName;
        std::string artistName;
        std::optional<core::UUID> recordingMBID;
        std::optional<core::UUID> trackMBID;
        std::optional<core::UUID> releaseMBID;
        std::optional<unsigned> trackNumber;
        Wt::WDateTime listenedAt;
    };

    std::ostream& operator<<(std::ostream& os, const Listen& listen);
} // namespace lms::scrobbling::listenBrainz
