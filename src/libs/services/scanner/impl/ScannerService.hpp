
#pragma once

#include <chrono>
#include <memory>
#include <optional>
#include <shared_mutex>
#include <vector>

#include <boost/asio/system_timer.hpp>

#include <Wt/WDateTime.h>
#include <Wt/WIOService.h>
#include <Wt/WSignal.h>

#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "services/scanner/IScannerService.hpp"

#include "FileScanners.hpp"
#include "ScannerSettings.hpp"
#include "steps/IScanStep.hpp"

namespace lms::core
{
    class IJobScheduler;
}

namespace lms::scanner
{
    class IFileScanner;

    // Main goals to keepthe scanner fast:
    // - single pass on files: only 1 filesystem exploration must be done (no further reads triggered by parsed values)
    // - stable: 1 single scan (full or not) is enough: successive scans must have no effect if there is no change in the files
    // ScannerService: 媒体库扫描服务的核心实现，负责调度定时/立即扫描并执行各个扫描步骤。
    // ScannerService: основная реализация сервиса сканирования медиатеки, планирует периодические/ручные запуски и выполняет шаги сканирования.
    class ScannerService : public IScannerService
    {
    public:
        ScannerService(db::IDb& db, const std::filesystem::path& cachePath);
        ~ScannerService() override;
        ScannerService(const ScannerService&) = delete;
        ScannerService& operator=(const ScannerService&) = delete;

    private:
        void requestReload() override;
        void requestImmediateScan(const ScanOptions& scanOptions) override;

        Status getStatus() const override;
        Events& getEvents() override { return _events; }

        void start();
        void stop();

        // Job 调度相关 / обработка фоновых задач
        void scheduleNextScan();
        void scheduleScan(const ScanOptions& scanOptions, const Wt::WDateTime& dateTime = {});

        void abortScan();

        // 定时回调：执行一次扫描并更新数据库。
        // Плановый коллбек: запускает сканирование и обновляет БД.
        void scan(const ScanOptions& scanOptions);
        void processScanSteps(ScanContext& context);

        void scanMediaDirectory(const std::filesystem::path& mediaDirectory, bool forceScan, ScanStats& stats);

        // Helpers
        void refreshScanSettings();
        void refreshTracingLoggerStats();

        void notifyInProgressIfNeeded(const ScanStepStats& stats);
        void notifyInProgress(const ScanStepStats& stats);

        db::IDb& _db;
        std::unique_ptr<core::IJobScheduler> _jobScheduler;
        const std::filesystem::path _cachePath;

        FileScanners _fileScanners;
        std::vector<std::unique_ptr<IScanStep>> _scanSteps;

        std::mutex _controlMutex;
        bool _abortScan{};
        Wt::WIOService _ioService;
        boost::asio::system_timer _scheduleTimer{ _ioService };
        Events _events;
        std::chrono::steady_clock::time_point _lastScanInProgressEmit;

        mutable std::shared_mutex _statusMutex;
        State _curState{ State::NotScheduled };
        std::optional<ScanStats> _lastCompleteScanStats;
        std::optional<ScanStepStats> _currentScanStepStats;
        Wt::WDateTime _nextScheduledScan;

        ScannerSettings _settings;
        std::optional<ScannerSettings> _lastScanSettings;
    };
} // namespace lms::scanner
