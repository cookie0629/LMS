#include "Utils.hpp"

#include <algorithm>
#include <filesystem>

#include "core/Path.hpp"
#include "core/String.hpp"

namespace lms::scanner::utils
{
    std::vector<std::filesystem::path> discoverFiles(
        const std::filesystem::path& rootPath,
        const std::vector<std::string>& extensions)
    {
        std::vector<std::filesystem::path> files;

        if (!std::filesystem::exists(rootPath) || !std::filesystem::is_directory(rootPath))
        {
            return files;
        }

        try
        {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(rootPath))
            {
                if (entry.is_regular_file())
                {
                    const auto& path = entry.path();
                    if (hasSupportedExtension(path, extensions))
                    {
                        files.push_back(path);
                    }
                }
            }
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            // 忽略权限错误等
        }

        return files;
    }

    bool hasSupportedExtension(
        const std::filesystem::path& filePath,
        const std::vector<std::string>& extensions)
    {
        if (extensions.empty())
        {
            return false;
        }

        std::string ext = core::pathUtils::getExtension(filePath);
        core::stringUtils::stringToLower(ext); // 就地修改字符串

        return std::find(extensions.begin(), extensions.end(), ext) != extensions.end();
    }
} // namespace lms::scanner::utils

