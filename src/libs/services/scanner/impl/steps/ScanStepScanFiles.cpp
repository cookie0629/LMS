#include "ScanStepScanFiles.hpp"

#include <chrono>
#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

#include "core/IConfig.hpp"
#include "core/ILogger.hpp"
#include "core/Path.hpp"
#include "core/Service.hpp"
#include "core/String.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/MediaLibrary.hpp"
#include "../scanners/FileToScan.hpp"
#include "../scanners/Utils.hpp"
#include "../scanners/audiofile/AudioFileScanner.hpp"
#include "services/scanner/ScanErrors.hpp"

namespace
{

    Wt::WDateTime toWDateTime(const std::filesystem::file_time_type& fileTime)
    {
        try
        {
            const auto fileNow = std::filesystem::file_time_type::clock::now();
            const auto sysNow = std::chrono::system_clock::now();
            const auto sysTime = sysNow + (fileTime - fileNow);
            return Wt::WDateTime::fromTime_t(std::chrono::system_clock::to_time_t(sysTime));
        }
        catch (...)
        {
            return {};
        }
    }

    void appendErrors(lms::scanner::ScanStats& stats, const lms::scanner::IFileScanOperation::ScanErrorVector& errors)
    {
        if (errors.empty())
        {
            return;
        }

        stats.errorsCount += errors.size();

        for (const auto& error : errors)
        {
            if (!error)
            {
                continue;
            }

            if (stats.errors.size() < lms::scanner::ScanStats::maxStoredErrorCount)
            {
                stats.errors.push_back(error);
            }
        }

        ++stats.failures;
    }

    void appendError(lms::scanner::ScanStats& stats, std::shared_ptr<lms::scanner::ScanError> error)
    {
        if (!error)
        {
            return;
        }

        if (stats.errors.size() < lms::scanner::ScanStats::maxStoredErrorCount)
        {
            stats.errors.push_back(std::move(error));
        }

        ++stats.errorsCount;
        ++stats.failures;
    }
} // namespace

namespace lms::scanner
{
    ScanStepScanFiles::ScanStepScanFiles(db::IDb& db, const ScannerSettings& settings)
        : ScanStepBase{ db, settings }
    {
    }

    bool ScanStepScanFiles::execute(ScanStats& stats)
    {
        LMS_LOG(SCANNER, INFO, "Starting file scan step");

        try
        {
            // 获取媒体库路径
            std::filesystem::path mediaLibraryPath;
            if (auto* config = lms::core::Service<lms::core::IConfig>::get())
            {
                mediaLibraryPath = config->getPath("media-library-path", std::filesystem::path{});
            }

            if (mediaLibraryPath.empty())
            {
                LMS_LOG(SCANNER, WARNING, "Media library path not configured (key: media-library-path)");
                return true; // 不是错误，只是未配置
            }

            if (!std::filesystem::exists(mediaLibraryPath))
            {
                LMS_LOG(SCANNER, WARNING, "Media library path does not exist: " << mediaLibraryPath);
                return true;
            }

            auto& session = getDb().getTLSSession();
            db::MediaLibrary::pointer mediaLibraryPtr;
            {
                auto readTx = session.createReadTransaction();
                mediaLibraryPtr = db::MediaLibrary::find(session, mediaLibraryPath);
                readTx.commit();
            }

            MediaLibraryInfo mediaLibraryInfo{
                .rootPath = mediaLibraryPath,
                .id = mediaLibraryPtr ? mediaLibraryPtr->getId() : db::MediaLibraryId{}
            };

            AudioFileScanner audioScanner(getDb(), getScannerSettings());

            auto supportedExtensions = audioScanner.getSupportedExtensions();
            std::vector<std::string> extensions;
            extensions.reserve(supportedExtensions.size());
            for (const auto& ext : supportedExtensions)
            {
                extensions.push_back(core::stringUtils::stringToLower(ext.string()));
            }

            LMS_LOG(SCANNER, INFO, "Discovering files in: " << mediaLibraryPath);
            auto files = utils::discoverFiles(mediaLibraryPath, extensions, &kExcludeDirFileName);
            stats.totalFileCount = files.size();
            LMS_LOG(SCANNER, INFO, "Found " << files.size() << " candidate audio files");

            for (const auto& filePath : files)
            {
                FileToScan file;
                file.filePath = filePath;
                file.mediaLibrary = mediaLibraryInfo;

                std::error_code ec;
                const auto fs = std::filesystem::file_size(filePath, ec);
                if (ec)
                {
                    appendError(stats, std::make_shared<FileScanError>(filePath, "Unable to read file size: " + ec.message()));
                    continue;
                }

                file.fileSize = static_cast<std::size_t>(fs);

                const auto writeTime = std::filesystem::last_write_time(filePath, ec);
                if (ec)
                {
                    appendError(stats, std::make_shared<FileScanError>(filePath, "Unable to read last write time: " + ec.message()));
                    continue;
                }

                file.lastWriteTime = toWDateTime(writeTime);

                if (!audioScanner.needsScan(file))
                {
                    ++stats.skips;
                    continue;
                }

                auto operation = audioScanner.createScanOperation(std::move(file));
                if (!operation)
                {
                    appendError(stats, std::make_shared<FileScanError>(filePath, "Unable to create scan operation"));
                    continue;
                }

                LMS_LOG(SCANNER, DEBUG, "Scanning file: " << filePath);

                operation->scan();
                const auto result = operation->processResult();
                ++stats.scans;

                switch (result)
                {
                case IFileScanOperation::OperationResult::Added:
                    ++stats.additions;
                    break;
                case IFileScanOperation::OperationResult::Updated:
                    ++stats.updates;
                    break;
                case IFileScanOperation::OperationResult::Removed:
                    ++stats.deletions;
                    break;
                case IFileScanOperation::OperationResult::Skipped:
                    ++stats.skips;
                    break;
                }

                appendErrors(stats, operation->getErrors());
            }

            LMS_LOG(SCANNER, INFO, "File scan step completed: "
                << stats.scans << " scanned, "
                << stats.additions << " added, "
                << stats.updates << " updated, "
                << stats.failures << " failures");
        }
        catch (const std::exception& e)
        {
            LMS_LOG(SCANNER, ERROR, "File scan step failed: " << e.what());
            return false;
        }

        return true;
    }
} // namespace lms::scanner

