
#pragma once

#include <string>
#include <variant>
#include <vector>

#include "core/EnumSet.hpp"

namespace lms::ui
{
    struct PicardReleaseType
    {
        // see https://musicbrainz.org/doc/Release_Group/Type
        enum class PrimaryType : std::uint8_t
        {
            Album,
            Single,
            EP,
            Broadcast,
            Other
        };

        enum class SecondaryType : std::uint8_t
        {
            Compilation,
            Soundtrack,
            Spokenword,
            Interview,
            Audiobook,
            AudioDrama,
            Live,
            Remix,
            DJMix,
            Mixtape_Street,
            Demo,
            FieldRecording,
        };

        PrimaryType primaryType;
        core::EnumSet<SecondaryType> secondaryTypes;

        bool operator<(const PicardReleaseType&) const;
    };

    struct CustomReleaseType
    {
        std::vector<std::string> types;

        bool operator<(const CustomReleaseType&) const;
    };

    using ReleaseType = std::variant<PicardReleaseType, CustomReleaseType>;
    ReleaseType parseReleaseType(const std::vector<std::string>& releaseTypeNames);
} // namespace lms::ui