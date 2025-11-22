#include "core/Path.hpp"

#include <filesystem>
#include <algorithm>
#include <array>

namespace lms::core::pathUtils
{
    bool exists(const std::filesystem::path& path)
    {
        std::error_code ec;
        return std::filesystem::exists(path, ec);
    }

    bool isDirectory(const std::filesystem::path& path)
    {
        std::error_code ec;
        return std::filesystem::is_directory(path, ec);
    }

    bool isFile(const std::filesystem::path& path)
    {
        std::error_code ec;
        return std::filesystem::is_regular_file(path, ec);
    }

    std::string getFilename(const std::filesystem::path& path)
    {
        return path.filename().string();
    }

    std::string getExtension(const std::filesystem::path& path)
    {
        return path.extension().string();
    }

    std::filesystem::path getParent(const std::filesystem::path& path)
    {
        return path.parent_path();
    }

    std::filesystem::path combine(const std::filesystem::path& p1, const std::filesystem::path& p2)
    {
        return p1 / p2;
    }

    bool hasFileAnyExtension(const std::filesystem::path& file, std::span<const std::filesystem::path> extensions)
    {
        const std::filesystem::path fileExtension = file.extension();
        
        for (const auto& ext : extensions)
        {
            // 转换为小写比较（跨平台兼容）
            std::string fileExtLower = fileExtension.string();
            std::string extLower = ext.string();
            
            std::transform(fileExtLower.begin(), fileExtLower.end(), fileExtLower.begin(), ::tolower);
            std::transform(extLower.begin(), extLower.end(), extLower.begin(), ::tolower);
            
            if (fileExtLower == extLower)
                return true;
        }
        
        return false;
    }

    std::string sanitizeFileStem(std::string_view fileStem)
    {
        // 非法字符列表（Windows和Linux都不允许的字符）
        constexpr std::array<char, 9> illegalChars = {'/', '\\', ':', '*', '?', '"', '<', '>', '|'};
        
        std::string sanitized;
        sanitized.reserve(fileStem.size());
        
        for (char c : fileStem)
        {
            // 检查是否为非法字符
            if (std::find(illegalChars.begin(), illegalChars.end(), c) == illegalChars.end())
            {
                sanitized.push_back(c);
            }
        }
        
        return sanitized;
    }
} // namespace lms::core::pathUtils

