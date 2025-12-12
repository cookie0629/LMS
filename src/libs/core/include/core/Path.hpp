
#pragma once

#include <filesystem>
#include <span>
#include <string>
#include <string_view>

namespace lms::core::pathUtils
{
    // Check if file's extension is one of provided extensions
    bool hasFileAnyExtension(const std::filesystem::path& file, std::span<const std::filesystem::path> extensions);

    // Check if a path is within a directory (excludeDirFileName is a relative can be used to exclude a whole directory and its subdirectory, must not have parent_path)
    // Caller responsibility to call with normalized paths
    bool isPathInRootPath(const std::filesystem::path& path, const std::filesystem::path& rootPath, const std::filesystem::path* excludeDirFileName = {});

    std::filesystem::path getLongestCommonPath(const std::filesystem::path& path1, const std::filesystem::path& path2);

    template<typename Iterator>
    std::filesystem::path getLongestCommonPath(Iterator first, Iterator last)
    {
        std::filesystem::path longestCommonPath;

        if (first == last)
            return longestCommonPath;

        longestCommonPath = *first++;

        while (first != last)
            longestCommonPath = core::pathUtils::getLongestCommonPath(*first++, longestCommonPath);

        return longestCommonPath;
    }

    // A method that sanitize a file stem, removing any illegal chars
    std::string sanitizeFileStem(std::string_view fileStem);
} // namespace lms::core::pathUtils
