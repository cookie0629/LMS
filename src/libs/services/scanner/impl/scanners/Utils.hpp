#pragma once

#include <filesystem>
#include <vector>

namespace lms::scanner::utils
{
    std::vector<std::filesystem::path> discoverFiles(
        const std::filesystem::path& rootPath,
        const std::vector<std::string>& extensions,
        const std::filesystem::path* excludeDirFileName = nullptr);

    bool hasSupportedExtension(
        const std::filesystem::path& filePath,
        const std::vector<std::string>& extensions);
} // namespace lms::scanner::utils

