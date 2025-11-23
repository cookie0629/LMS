#pragma once

#include <filesystem>

#include "database/objects/MediaLibraryId.hpp"

namespace lms::scanner
{
    /**
     * @brief 媒体库信息（简化版）
     */
    struct MediaLibraryInfo
    {
        std::filesystem::path rootPath;  // 媒体库根路径
        lms::db::MediaLibraryId id;      // 媒体库 ID（如果已存在）
    };
} // namespace lms::scanner

