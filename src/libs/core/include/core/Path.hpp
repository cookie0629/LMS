#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <span>

namespace lms::core::pathUtils
{
    /**
     * @brief 检查路径是否存在
     * @param path 要检查的路径
     * @return true 如果路径存在，false 否则
     */
    bool exists(const std::filesystem::path& path);

    /**
     * @brief 检查路径是否为目录
     * @param path 要检查的路径
     * @return true 如果是目录，false 否则
     */
    bool isDirectory(const std::filesystem::path& path);

    /**
     * @brief 检查路径是否为普通文件
     * @param path 要检查的路径
     * @return true 如果是普通文件，false 否则
     */
    bool isFile(const std::filesystem::path& path);

    /**
     * @brief 获取文件名（不含路径）
     * @param path 文件路径
     * @return 文件名
     */
    std::string getFilename(const std::filesystem::path& path);

    /**
     * @brief 获取文件扩展名（包含点号，如 .mp3）
     * @param path 文件路径
     * @return 扩展名，如果没有则返回空字符串
     */
    std::string getExtension(const std::filesystem::path& path);

    /**
     * @brief 获取父目录路径
     * @param path 文件或目录路径
     * @return 父目录路径
     */
    std::filesystem::path getParent(const std::filesystem::path& path);

    /**
     * @brief 拼接两个路径
     * @param p1 第一个路径
     * @param p2 第二个路径
     * @return 拼接后的路径
     */
    std::filesystem::path combine(const std::filesystem::path& p1, const std::filesystem::path& p2);

    /**
     * @brief 检查文件是否有指定的扩展名之一
     * @param file 文件路径
     * @param extensions 扩展名列表（如 {".mp3", ".flac", ".ogg"}）
     * @return true 如果文件有任一扩展名，false 否则
     */
    bool hasFileAnyExtension(const std::filesystem::path& file, std::span<const std::filesystem::path> extensions);

    /**
     * @brief 清理文件名，移除非法字符
     * @param fileStem 文件名（不含扩展名）
     * @return 清理后的文件名
     */
    std::string sanitizeFileStem(std::string_view fileStem);
} // namespace lms::core::pathUtils

