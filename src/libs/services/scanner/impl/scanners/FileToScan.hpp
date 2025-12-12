
#pragma once

#include <filesystem>

#include <Wt/WDateTime.h>

#include "MediaLibraryInfo.hpp"

namespace lms::scanner
{
    struct FileToScan
    {
        std::filesystem::path filePath;
        MediaLibraryInfo mediaLibrary;
        Wt::WDateTime lastWriteTime;
        std::size_t fileSize{};
    };
} // namespace lms::scanner
