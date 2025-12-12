
#pragma once

#include "audio/IAudioFileInfo.hpp"

#include "scanners/IFileScanner.hpp"
#include "scanners/audiofile/TrackMetadataParser.hpp"

namespace lms
{
    namespace db
    {
        class IDb;
    }

    namespace metadata
    {
        class IAudioFileParser;
    }
} // namespace lms

namespace lms::scanner
{
    struct ScannerSettings;

    class AudioFileScanner : public IFileScanner
    {
    public:
        AudioFileScanner(db::IDb& db, const ScannerSettings& settings);
        ~AudioFileScanner() override;
        AudioFileScanner(const AudioFileScanner&) = delete;
        AudioFileScanner& operator=(const AudioFileScanner&) = delete;

    private:
        core::LiteralString getName() const override;
        std::span<const std::filesystem::path> getSupportedFiles() const override;
        std::span<const std::filesystem::path> getSupportedExtensions() const override;
        bool needsScan(const FileToScan& file) const override;
        std::unique_ptr<IFileScanOperation> createScanOperation(FileToScan&& fileToScan) const override;

        db::IDb& _db;
        const ScannerSettings& _settings;
        const TrackMetadataParser _trackMetadataParser;
        const audio::ParserOptions _parserOptions;
    };
} // namespace lms::scanner