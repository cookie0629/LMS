#pragma once

#include <filesystem>
#include <memory>
#include <optional>

#include "ScannerEvents.hpp"
#include "ScannerOptions.hpp"
#include "ScannerStats.hpp"

namespace lms::db
{
    class IDb;
} // namespace lms::db

namespace lms::scanner
{
    /**
     * @brief 扫描服务接口
     */
    class IScannerService
    {
    public:
        virtual ~IScannerService() = default;

        /**
         * @brief 扫描状态
         */
        enum class State
        {
            NotScheduled,  // 未调度
            Scheduled,     // 已调度
            InProgress,    // 进行中
        };

        /**
         * @brief 扫描服务状态
         */
        struct Status
        {
            State currentState{ State::NotScheduled };
            Wt::WDateTime nextScheduledScan;
            std::optional<ScanStats> lastCompleteScanStats;
            std::optional<ScanStepStats> currentScanStepStats;
        };

        /**
         * @brief 请求重新加载（将停止/重新调度扫描）
         */
        virtual void requestReload() = 0;

        /**
         * @brief 请求立即扫描
         * @param options 扫描选项
         */
        virtual void requestImmediateScan(const ScanOptions& options = {}) = 0;

        /**
         * @brief 获取当前状态
         */
        virtual Status getStatus() const = 0;

        /**
         * @brief 获取事件对象
         */
        virtual Events& getEvents() = 0;
    };

    /**
     * @brief 创建扫描服务
     * @param db 数据库引用
     * @param cachePath 缓存路径
     * @return 扫描服务实例
     */
    std::unique_ptr<IScannerService> createScannerService(
        db::IDb& db,
        const std::filesystem::path& cachePath);
} // namespace lms::scanner

