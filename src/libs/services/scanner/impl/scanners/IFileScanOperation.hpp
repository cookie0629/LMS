#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include "services/scanner/ScanErrors.hpp"

namespace lms::scanner
{
    /**
     * @brief 文件扫描操作接口
     */
    class IFileScanOperation
    {
    public:
        virtual ~IFileScanOperation() = default;

        /**
         * @brief 获取操作名称
         */
        virtual std::string_view getName() const = 0;

        /**
         * @brief 获取文件路径
         */
        virtual const std::filesystem::path& getFilePath() const = 0;

        /**
         * @brief 执行扫描（异步调用）
         */
        virtual void scan() = 0;

        /**
         * @brief 操作结果
         */
        enum class OperationResult
        {
            Added,    // 已添加
            Removed,  // 已删除
            Updated,  // 已更新
            Skipped,  // 已跳过
        };

        /**
         * @brief 处理扫描结果（顺序调用）
         */
        virtual OperationResult processResult() = 0;

        /**
         * @brief 获取扫描过程中收集的错误
         */
        using ScanErrorVector = std::vector<std::shared_ptr<ScanError>>;
        virtual const ScanErrorVector& getErrors() = 0;
    };
} // namespace lms::scanner

