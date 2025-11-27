#pragma once

#include <filesystem>
#include <vector>

namespace lms::scanner::utils
{
    /**
     * @brief 获取目录中的所有文件（递归）
     * @param rootPath 根目录路径
     * @param extensions 支持的扩展名列表（小写，包含点号，如 ".mp3"）
     * @return 文件路径列表
     */
    std::vector<std::filesystem::path> discoverFiles(
        const std::filesystem::path& rootPath,
        const std::vector<std::string>& extensions,
        const std::filesystem::path* excludeDirFileName = nullptr);

    /**
     * @brief 检查文件扩展名是否在支持列表中
     * @param filePath 文件路径
     * @param extensions 支持的扩展名列表（小写，包含点号）
     * @return true 如果支持，false 否则
     */
    bool hasSupportedExtension(
        const std::filesystem::path& filePath,
        const std::vector<std::string>& extensions);
} // namespace lms::scanner::utils

