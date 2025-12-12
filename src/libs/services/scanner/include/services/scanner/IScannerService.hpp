
#pragma once

#include <filesystem>
#include <optional>

#include "ScannerEvents.hpp"
#include "ScannerOptions.hpp"
#include "ScannerStats.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::scanner
{
    // IScannerService: 媒体库扫描服务接口，负责调度全库/增量扫描并对外暴露状态和事件。
    // IScannerService: интерфейс сервиса сканирования медиатеки, управляет полным/инкрементальным сканом и отдаёт статус/события.
    class IScannerService
    {
    public:
        virtual ~IScannerService() = default;

        // 异步请求：重载配置或立刻启动一次扫描。
        // Асинхронные запросы: перезагрузка настроек/очереди и немедленный запуск сканирования.
        virtual void requestReload() = 0; // 会停止当前扫描并重新调度 / остановит и переназначит сканирование
        virtual void requestImmediateScan(const ScanOptions& options = {}) = 0;

        enum class State
        {
            NotScheduled,
            Scheduled,
            InProgress,
        };

        struct Status
        {
            State currentState{ State::NotScheduled };
            Wt::WDateTime nextScheduledScan;
            std::optional<ScanStats> lastCompleteScanStats;
            std::optional<ScanStepStats> currentScanStepStats;
        };

        virtual Status getStatus() const = 0;

        virtual Events& getEvents() = 0;
    };

    std::unique_ptr<IScannerService> createScannerService(db::IDb& db, const std::filesystem::path& cachePath);
} // namespace lms::scanner
