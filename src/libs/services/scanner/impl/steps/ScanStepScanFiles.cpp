#include "ScanStepScanFiles.hpp"

#include <filesystem>

#include "core/IConfig.hpp"
#include "core/ILogger.hpp"
#include "core/Path.hpp"
#include "core/Service.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "../scanners/Utils.hpp"
#include "../scanners/audiofile/AudioFileScanner.hpp"
#include "../scanners/FileToScan.hpp"

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
            // 获取媒体库路径（从配置中读取，简化版使用固定路径）
            std::filesystem::path mediaLibraryPath;
            if (auto* config = lms::core::Service<lms::core::IConfig>::get())
            {
                mediaLibraryPath = config->getPath("media-library-path", std::filesystem::path{});
            }

            if (mediaLibraryPath.empty() || !std::filesystem::exists(mediaLibraryPath))
            {
                LMS_LOG(SCANNER, WARNING, "Media library path not configured or does not exist");
                return true; // 不是错误，只是没有配置
            }

            // 创建音频文件扫描器
            AudioFileScanner audioScanner(getDb(), getScannerSettings());

            // 获取支持的扩展名
            auto supportedExtensions = audioScanner.getSupportedExtensions();
            std::vector<std::string> extensions;
            for (const auto& ext : supportedExtensions)
            {
                extensions.push_back(ext.string());
            }

            // 发现文件
            LMS_LOG(SCANNER, INFO, "Discovering files in: " << mediaLibraryPath);
            auto files = utils::discoverFiles(mediaLibraryPath, extensions);
            stats.totalFileCount = files.size();
            LMS_LOG(SCANNER, INFO, "Found " << files.size() << " audio files");

            // 简化版：暂时只统计文件数量
            // 实际实现需要：
            // 1. 为每个文件创建 FileToScan 对象
            // 2. 使用 IFileScanner 扫描文件
            // 3. 更新数据库

            stats.scans = files.size();
            stats.skips = 0; // 简化版：不跳过任何文件

            LMS_LOG(SCANNER, INFO, "File scan step completed: " << files.size() << " files found");
        }
        catch (const std::exception& e)
        {
            LMS_LOG(SCANNER, ERROR, "File scan step failed: " << e.what());
            return false;
        }

        return true;
    }
} // namespace lms::scanner

