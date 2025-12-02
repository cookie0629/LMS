#include "ScannerService.hpp"

#include <thread>
#include <string_view>

#include "core/IConfig.hpp"
#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "core/String.hpp"
#include "database/Session.hpp"
#include "scanners/FileScanOperationBase.hpp"
#include "steps/ScanStepScanFiles.hpp"
#include "steps/ScanStepAssociateExternalLyrics.hpp"
#include "steps/ScanStepCheckForRemovedFiles.hpp"
#include "steps/ScanStepCheckForDuplicatedFiles.hpp"
#include "steps/ScanStepUpdateLibraryFields.hpp"
#include "steps/ScanStepRemoveOrphanedDbEntries.hpp"
#include "steps/ScanStepOptimize.hpp"
#include "steps/ScanStepCompact.hpp"

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

        auto buildScannerSettings = [&](ScannerSettings& settings) {
            settings.enableCompact = options.compact;
            settings.enableOptimize = true;
            settings.preferAlbumArtistFallback = true;

            if (auto* config = lms::core::Service<lms::core::IConfig>::get())
            {
                settings.enableOptimize = config->getBool("scanner.optimize-enabled", settings.enableOptimize);
                settings.enableCompact = settings.enableCompact || config->getBool("scanner.compact-enabled", false);
                settings.preferAlbumArtistFallback = config->getBool("scanner.metadata.prefer-album-artist-fallback", true);

                const auto loadDelimiters = [&](std::string_view key, std::vector<std::string>& target, const std::vector<std::string>& defaults) {
                    std::vector<std::string> values;
                    config->visitStrings(key, [&](std::string_view value) {
                        auto trimmed = std::string{ lms::core::stringUtils::stringTrim(value) };
                        if (!trimmed.empty())
                        {
                            values.emplace_back(std::move(trimmed));
                        }
                    }, {});

                    if (!values.empty())
                    {
                        target = std::move(values);
                    }
                    else if (target.empty())
                    {
                        target = defaults;
                    }
                };

                const auto artistDefaults = settings.metadataParserParameters.artistTagDelimiters;
                loadDelimiters("scanner.metadata.artist-delimiters", settings.metadataParserParameters.artistTagDelimiters, artistDefaults);

                const auto defaultDefaults = settings.metadataParserParameters.defaultTagDelimiters;
                loadDelimiters("scanner.metadata.default-delimiters", settings.metadataParserParameters.defaultTagDelimiters, defaultDefaults);

                const auto loadList = [&](std::string_view key, std::vector<std::string>& target) {
                    std::vector<std::string> values;
                    config->visitStrings(key, [&](std::string_view value) {
                        auto trimmed = std::string{ lms::core::stringUtils::stringTrim(value) };
                        if (!trimmed.empty())
                        {
                            values.emplace_back(std::move(trimmed));
                        }
                    }, {});

                    if (!values.empty())
                    {
                        target = std::move(values);
                    }
                };

                loadList("scanner.metadata.artists-to-not-split", settings.metadataParserParameters.artistsToNotSplit);
            }

            if (options.forceOptimize)
            {
                settings.enableOptimize = true;
            }
            if (options.compact)
            {
                settings.enableCompact = true;
            }
        };

        ScannerSettings settings;
        buildScannerSettings(settings);
        
        // 步骤 1: 扫描文件（音频 + 外部歌词）
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

        // 步骤 2: 关联外部歌词到 Track
        {
            ScanStepAssociateExternalLyrics associateLyricsStep(_db, settings);
            if (!associateLyricsStep.execute(stats))
            {
                LMS_LOG(SCANNER, ERROR, "Associate external lyrics step failed");
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

        // 步骤 3: 检查已删除的文件
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

        // 步骤 4: 检查重复文件
        {
            ScanStepCheckForDuplicatedFiles checkDuplicatedStep(_db, settings);
            if (!checkDuplicatedStep.execute(stats))
            {
                LMS_LOG(SCANNER, ERROR, "Check for duplicated files step failed");
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

        // 步骤 5: 更新媒体库字段
        {
            ScanStepUpdateLibraryFields updateLibraryStep(_db, settings);
            if (!updateLibraryStep.execute(stats))
            {
                LMS_LOG(SCANNER, ERROR, "Update library fields step failed");
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

        // 步骤 6: 删除孤立数据库条目
        {
            ScanStepRemoveOrphanedDbEntries removeOrphanedStep(_db, settings);
            if (!removeOrphanedStep.execute(stats))
            {
                LMS_LOG(SCANNER, ERROR, "Remove orphaned DB entries step failed");
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

        // 步骤 7: 数据库优化
        {
            ScanStepOptimize optimizeStep(_db, settings);
            if (!optimizeStep.execute(stats))
            {
                LMS_LOG(SCANNER, ERROR, "Optimize database step failed");
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

        // 步骤 8: 数据库压缩
        {
            ScanStepCompact compactStep(_db, settings);
            if (!compactStep.execute(stats))
            {
                LMS_LOG(SCANNER, ERROR, "Compact database step failed");
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

