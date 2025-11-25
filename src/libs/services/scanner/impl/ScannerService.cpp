#include "ScannerService.hpp"

#include <thread>

#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "database/Session.hpp"
#include "scanners/FileScanOperationBase.hpp"
#include "steps/ScanStepScanFiles.hpp"
#include "steps/ScanStepCheckForRemovedFiles.hpp"
#include "steps/ScanStepCheckForDuplicatedFiles.hpp"

namespace lms::scanner
{
    std::unique_ptr<IScannerService> createScannerService(
        db::IDb& db,
        const std::filesystem::path& cachePath)
    {
        return std::make_unique<ScannerService>(db, cachePath);
    }

    ScannerService::ScannerService(db::IDb& db, const std::filesystem::path& cachePath)
        : _db{ db }
        , _cachePath{ cachePath }
    {
        if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
        {
            LMS_LOG(SCANNER, INFO, "Scanner Service initialized, cache path: " << cachePath);
        }
    }

    ScannerService::~ScannerService()
    {
        {
            std::lock_guard lock(_mutex);
            _stopRequested = true;
        }

        if (_scanThread.joinable())
        {
            _scanThread.join();
        }
    }

    void ScannerService::requestReload()
    {
        std::lock_guard lock(_mutex);
        
        // 停止当前扫描
        _stopRequested = true;
        _currentState = State::NotScheduled;
        _nextScheduledScan = Wt::WDateTime{};

        if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
        {
            LMS_LOG(SCANNER, INFO, "Scanner reload requested");
        }
    }

    void ScannerService::requestImmediateScan(const ScanOptions& options)
    {
        std::lock_guard lock(_mutex);

        if (_currentState == State::InProgress)
        {
            if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
            {
                LMS_LOG(SCANNER, WARNING, "Scan already in progress, ignoring request");
            }
            return;
        }

        // 如果扫描线程正在运行，等待它完成
        if (_scanThread.joinable())
        {
            _scanThread.join();
        }

        _currentState = State::InProgress;
        _stopRequested = false;

        // 启动新的扫描线程
        _scanThread = std::thread([this, options]() {
            this->scanThreadFunc();
        });

        if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
        {
            LMS_LOG(SCANNER, INFO, "Immediate scan requested");
        }
    }

    IScannerService::Status ScannerService::getStatus() const
    {
        std::lock_guard lock(_mutex);

        Status status;
        status.currentState = _currentState;
        status.nextScheduledScan = _nextScheduledScan;
        status.lastCompleteScanStats = _lastCompleteScanStats;
        status.currentScanStepStats = _currentScanStepStats;

        return status;
    }

    Events& ScannerService::getEvents()
    {
        return _events;
    }

    void ScannerService::scanThreadFunc()
    {
        try
        {
            ScanOptions options;
            {
                std::lock_guard lock(_mutex);
                // 暂时使用默认选项
            }

            performScan(options);
        }
        catch (const std::exception& e)
        {
            if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
            {
                LMS_LOG(SCANNER, ERROR, "Scan thread exception: " << e.what());
            }

            std::lock_guard lock(_mutex);
            _currentState = State::NotScheduled;
        }
    }

    void ScannerService::performScan(const ScanOptions& options)
    {
        LMS_LOG(SCANNER, INFO, "Starting scan (fullScan=" << options.fullScan << ")");

        // 发送扫描开始事件
        _events.scanStarted.emit();

        ScanStats stats;
        stats.startTime = Wt::WDateTime::currentDateTime();

        // 简化版：暂时只记录开始和结束
        // 实际实现需要：
        // 1. 扫描媒体库目录
        // 2. 提取音频文件元数据
        // 3. 更新数据库
        // 4. 报告进度

        {
            std::lock_guard lock(_mutex);
            if (_stopRequested)
            {
                _events.scanAborted.emit();
                _currentState = State::NotScheduled;
                return;
            }
        }

        // 执行扫描步骤
        ScannerSettings settings;
        
        // 步骤 1: 扫描文件
        {
            ScanStepScanFiles scanFilesStep(_db, settings);
            if (!scanFilesStep.execute(stats))
            {
                LMS_LOG(SCANNER, ERROR, "File scan step failed");
            }
        }

        // 检查是否需要停止
        {
            std::lock_guard lock(_mutex);
            if (_stopRequested)
            {
                _events.scanAborted.emit();
                _currentState = State::NotScheduled;
                return;
            }
        }

        // 步骤 2: 检查已删除的文件
        {
            ScanStepCheckForRemovedFiles checkRemovedStep(_db, settings);
            if (!checkRemovedStep.execute(stats))
            {
                LMS_LOG(SCANNER, ERROR, "Check for removed files step failed");
            }
        }

        // 再次检查是否需要停止
        {
            std::lock_guard lock(_mutex);
            if (_stopRequested)
            {
                _events.scanAborted.emit();
                _currentState = State::NotScheduled;
                return;
            }
        }

        // 步骤 3: 检查重复文件
        {
            ScanStepCheckForDuplicatedFiles checkDuplicatedStep(_db, settings);
            if (!checkDuplicatedStep.execute(stats))
            {
                LMS_LOG(SCANNER, ERROR, "Check for duplicated files step failed");
            }
        }

        stats.stopTime = Wt::WDateTime::currentDateTime();

        {
            std::lock_guard lock(_mutex);
            _lastCompleteScanStats = stats;
            _currentScanStepStats = std::nullopt;
            _currentState = State::NotScheduled;
        }

        // 发送扫描完成事件
        _events.scanComplete.emit(stats);

        LMS_LOG(SCANNER, INFO, "Scan completed");
    }
} // namespace lms::scanner

