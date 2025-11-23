#pragma once

#include <filesystem>
#include <memory>
#include <span>

#include "FileToScan.hpp"
#include "IFileScanOperation.hpp"

namespace lms::scanner
{
    /**
     * @brief 文件扫描器接口
     */
    class IFileScanner
    {
    public:
        virtual ~IFileScanner() = default;

        /**
         * @brief 获取扫描器名称
         */
        virtual std::string_view getName() const = 0;

        /**
         * @brief 获取支持的文件列表（完整文件名匹配）
         */
        virtual std::span<const std::filesystem::path> getSupportedFiles() const = 0;

        /**
         * @brief 获取支持的扩展名列表
         */
        virtual std::span<const std::filesystem::path> getSupportedExtensions() const = 0;

        /**
         * @brief 检查文件是否需要扫描
         * @param file 待扫描的文件
         * @return true 如果需要扫描，false 否则
         */
        virtual bool needsScan(const FileToScan& file) const = 0;

        /**
         * @brief 创建扫描操作
         * @param fileToScan 待扫描的文件
         * @return 扫描操作对象
         */
        virtual std::unique_ptr<IFileScanOperation> createScanOperation(FileToScan&& fileToScan) const = 0;
    };
} // namespace lms::scanner

