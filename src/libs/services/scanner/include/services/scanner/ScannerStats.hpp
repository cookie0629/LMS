#pragma once

#include <Wt/WDateTime.h>

#include <memory>
#include <vector>

#include "ScanErrors.hpp"

namespace lms::scanner
{
    /**
     * @brief 重复原因
     */
    enum class DuplicateReason
    {
        SameHash,      // 相同哈希值
        SameTrackMBID, // 相同 MusicBrainz ID
    };

    /**
     * @brief 扫描发现的重复文件
     */
    struct ScanDuplicate
    {
        // 注意：这里暂时不使用 TrackId，因为 Track 对象还未实现
        // db::TrackId trackId;
        std::int64_t trackId{};
        DuplicateReason reason;
    };

    /**
     * @brief 扫描步骤
     */
    enum class ScanStep
    {
        ScanFiles,               // 扫描文件
        AssociateExternalLyrics, // 关联外部歌词
        CheckForRemovedFiles,    // 检查已删除的文件
        CheckForDuplicatedFiles, // 检查重复文件
        UpdateLibraryFields,     // 更新媒体库字段
        RemoveOrphanedDbEntries, // 删除孤立的数据库条目
        Optimize,                // 优化数据库
        Compact,                 // 压缩数据库
    };

    /**
     * @brief 扫描步骤统计
     */
    struct ScanStepStats
    {
        Wt::WDateTime startTime;

        std::size_t stepCount{};    // 总步骤数
        std::size_t stepIndex{};    // 当前步骤索引
        ScanStep currentStep;       // 当前步骤

        std::size_t totalElems{};   // 总元素数
        std::size_t processedElems{}; // 已处理元素数

        /**
         * @brief 计算进度百分比
         */
        unsigned progress() const
        {
            if (totalElems == 0)
                return 0;
            return static_cast<unsigned>((processedElems * 100) / totalElems);
        }
    };

    /**
     * @brief 扫描统计信息
     */
    struct ScanStats
    {
        Wt::WDateTime startTime;
        Wt::WDateTime stopTime;

        std::size_t totalFileCount{}; // 总文件数（仅在文件扫描步骤后有效）

        std::size_t skips{};    // 跳过的文件数（自上次扫描后未更改）
        std::size_t scans{};    // 扫描的文件数
        std::size_t additions{}; // 添加到数据库的文件数
        std::size_t deletions{}; // 从数据库删除的文件数
        std::size_t updates{};   // 更新的文件数
        std::size_t failures{};  // 扫描失败的文件数

        static constexpr std::size_t maxStoredErrorCount{ 5000 };
        std::vector<std::shared_ptr<ScanError>> errors;
        std::size_t errorsCount{}; // 可能大于 errors.size()，如果错误太多
        std::vector<ScanDuplicate> duplicates;

        /**
         * @brief 获取总文件数
         */
        std::size_t getTotalFileCount() const
        {
            return totalFileCount;
        }

        /**
         * @brief 获取变更总数
         */
        std::size_t getChangesCount() const
        {
            return additions + deletions + updates;
        }
    };
} // namespace lms::scanner

