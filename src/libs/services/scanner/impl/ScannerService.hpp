#pragma once

#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>

#include "services/scanner/IScannerService.hpp"

namespace lms::db
{
    class IDb;
    class Session;
} // namespace lms::db

namespace lms::scanner
{
    /**
     * @brief 扫描服务实现（简化版）
     */
    class ScannerService : public IScannerService
    {
    public:
        ScannerService(db::IDb& db, const std::filesystem::path& cachePath);
        ~ScannerService() override;

        ScannerService(const ScannerService&) = delete;
        ScannerService& operator=(const ScannerService&) = delete;
        ScannerService(ScannerService&&) = delete;
        ScannerService& operator=(ScannerService&&) = delete;

        void requestReload() override;
        void requestImmediateScan(const ScanOptions& options = {}) override;
        Status getStatus() const override;
        Events& getEvents() override;

    private:
        void scanThreadFunc();
        void performScan(const ScanOptions& options);

        db::IDb& _db;
        std::filesystem::path _cachePath;

        mutable std::mutex _mutex;
        State _currentState{ State::NotScheduled };
        Wt::WDateTime _nextScheduledScan;
        std::optional<ScanStats> _lastCompleteScanStats;
        std::optional<ScanStepStats> _currentScanStepStats;
        Events _events;

        std::thread _scanThread;
        bool _stopRequested{ false };
    };
} // namespace lms::scanner

