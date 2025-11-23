#include "AudioFileScanner.hpp"

#include <algorithm>
#include <filesystem>

#include "core/Path.hpp"
#include "../IFileScanOperation.hpp"
#include "../FileScanOperationBase.hpp"
#include "AudioFileScanOperation.hpp"

namespace lms::scanner
{
    // 支持的音频文件扩展名（小写，包含点号）
    const std::vector<std::filesystem::path> AudioFileScanner::_supportedExtensions = {
        ".mp3",
        ".flac",
        ".ogg",
        ".m4a",
        ".aac",
        ".wav",
        ".wma",
        ".opus",
        ".mpc",
        ".ape",
    };

    AudioFileScanner::AudioFileScanner(db::IDb& db, const ScannerSettings& settings)
        : _db{ db }
        , _settings{ settings }
    {
    }

    std::string_view AudioFileScanner::getName() const
    {
        return "Audio scanner";
    }

    std::span<const std::filesystem::path> AudioFileScanner::getSupportedFiles() const
    {
        return {};
    }

    std::span<const std::filesystem::path> AudioFileScanner::getSupportedExtensions() const
    {
        return _supportedExtensions;
    }

    bool AudioFileScanner::needsScan(const FileToScan& file) const
    {
        // 简化版：总是需要扫描
        // 实际实现需要检查文件是否已更改
        (void)file;
        return true;
    }

    std::unique_ptr<IFileScanOperation> AudioFileScanner::createScanOperation(FileToScan&& fileToScan) const
    {
        return std::make_unique<AudioFileScanOperation>(
            std::move(fileToScan),
            _db,
            _settings);
    }
} // namespace lms::scanner

