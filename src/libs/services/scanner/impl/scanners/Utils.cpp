#include "Utils.hpp"

#include <algorithm>
#include <filesystem>
#include <system_error>

#include "core/Path.hpp"
#include "core/String.hpp"

namespace lms::scanner::utils
{
    namespace
    {
        bool shouldSkipDirectory(const std::filesystem::path& directory, const std::filesystem::path* excludeDirFileName)
        {
            if (!excludeDirFileName || excludeDirFileName->empty())
            {
                return false;
            }

            std::error_code ec;
            const auto marker = directory / *excludeDirFileName;
            return std::filesystem::exists(marker, ec) && std::filesystem::is_regular_file(marker, ec);
        }
    } // namespace

    std::vector<std::filesystem::path> discoverFiles(
        const std::filesystem::path& rootPath,
        const std::vector<std::string>& extensions,
        const std::filesystem::path* excludeDirFileName)
    {
        std::vector<std::filesystem::path> files;

        if (!std::filesystem::exists(rootPath) || !std::filesystem::is_directory(rootPath))
        {
            return files;
        }

        try
        {
            std::error_code ec;
            std::filesystem::recursive_directory_iterator it(
                rootPath,
                std::filesystem::directory_options::skip_permission_denied,
                ec);

            if (ec)
            {
                return files;
            }

            const std::filesystem::recursive_directory_iterator end;
            for (; it != end; ++it)
            {
                const auto& entry = *it;
                ec.clear();
                if (entry.is_directory(ec))
                {
                    if (shouldSkipDirectory(entry.path(), excludeDirFileName))
                    {
                        it.disable_recursion_pending();
                    }
                    continue;
                }

                ec.clear();
                if (entry.is_regular_file(ec))
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

