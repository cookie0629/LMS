
#pragma once

#include <filesystem>

#include "database/objects/MediaLibraryId.hpp"

namespace lms::scanner
{
    struct MediaLibraryInfo
    {
        db::MediaLibraryId id;
        std::filesystem::path rootDirectory;
        bool firstScan{};

        auto operator<=>(const MediaLibraryInfo& other) const = default;
    };
} // namespace lms::scanner