
#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <Wt/WDateTime.h>

#include "database/objects/ScanSettings.hpp"

#include "MediaLibraryInfo.hpp"

namespace lms::scanner
{
    // 在某个目录中放置该文件时，该目录会被扫描器忽略。
    // Если в каталоге есть этот файл, сканер будет игнорировать этот каталог.
    static inline const std::filesystem::path excludeDirFileName{ ".lmsignore" };

    // ScannerSettings: 从数据库/配置中聚合出来的扫描配置（时间、周期、标签分隔符、媒体库列表等）。
    // ScannerSettings: агрегированные настройки сканера (время, период, разделители тегов, список медиатек и т.д.).
    struct ScannerSettings
    {
        std::size_t audioScanVersion{};
        std::size_t artistInfoScanVersion{};
        Wt::WTime startTime;
        db::ScanSettings::UpdatePeriod updatePeriod{ db::ScanSettings::UpdatePeriod::Never };
        bool skipDuplicateTrackMBID{};
        std::vector<std::string> extraTags;
        std::vector<std::string> artistTagDelimiters;
        std::vector<std::string> artistsToNotSplit;
        std::vector<std::string> defaultTagDelimiters;
        bool skipSingleReleasePlayLists{};
        bool allowArtistMBIDFallback{ true };
        bool artistImageFallbackToRelease{};

        std::vector<MediaLibraryInfo> mediaLibraries;

        bool operator==(const ScannerSettings& rhs) const = default;
    };
} // namespace lms::scanner
