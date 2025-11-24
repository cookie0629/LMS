#include "AudioFileScanner.hpp"

#include <algorithm>
#include <filesystem>

#include "core/ILogger.hpp"
#include "core/Path.hpp"
#include "../IFileScanOperation.hpp"
#include "../FileScanOperationBase.hpp"
#include "AudioFileScanOperation.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/Track.hpp"

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
        try
        {
            auto& session = _db.getTLSSession();
            auto readTransaction = session.createReadTransaction();
            auto existingTrack = db::Track::findByPath(session, file.filePath);
            readTransaction.commit();

            if (!existingTrack)
            {
                return true;
            }

            if (existingTrack->getFileSize() != static_cast<long long>(file.fileSize))
            {
                return true;
            }

            const auto storedWriteTime = existingTrack->getLastWriteTime();
            if (!storedWriteTime.isValid() || !file.lastWriteTime.isValid())
            {
                return true;
            }

            if (storedWriteTime != file.lastWriteTime)
            {
                return true;
            }

            return false;
        }
        catch (const std::exception& e)
        {
            LMS_LOG(SCANNER, WARNING, "Failed to determine if file needs scan (" << file.filePath << "): " << e.what());
            return true;
        }
    }

    std::unique_ptr<IFileScanOperation> AudioFileScanner::createScanOperation(FileToScan&& fileToScan) const
    {
        return std::make_unique<AudioFileScanOperation>(
            std::move(fileToScan),
            _db,
            _settings);
    }
} // namespace lms::scanner

