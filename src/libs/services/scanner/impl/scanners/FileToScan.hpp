#pragma once

#include <filesystem>

#include <Wt/WDateTime.h>

#include "../MediaLibraryInfo.hpp"

namespace lms::scanner
{
    /**
     * @brief 待扫描的文件信息
     */
    struct FileToScan
    {
        std::filesystem::path filePath;      // 文件路径
        MediaLibraryInfo mediaLibrary;       // 所属媒体库信息
        Wt::WDateTime lastWriteTime;         // 最后修改时间
        std::size_t fileSize{};              // 文件大小
    };
} // namespace lms::scanner

