#pragma once

#include <filesystem>

namespace lms::scanner
{
    /**
     * @brief 媒体库信息（简化版）
     * 注意：完整版本需要包含 MediaLibrary ID 等信息
     */
    struct MediaLibraryInfo
    {
        std::filesystem::path rootPath;  // 媒体库根路径
        // 注意：完整版本还需要 MediaLibrary ID 等
    };
} // namespace lms::scanner

