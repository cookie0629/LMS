
#pragma once

#include "scanners/IFileScanner.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::scanner
{
    struct ScannerSettings;

    class LyricsFileScanner : public IFileScanner
    {
    public:
        LyricsFileScanner(db::IDb& db, ScannerSettings& settings);
        ~LyricsFileScanner() override = default;
        LyricsFileScanner(const LyricsFileScanner&) = delete;
        LyricsFileScanner& operator=(const LyricsFileScanner&) = delete;

    private:
        core::LiteralString getName() const override;
        std::span<const std::filesystem::path> getSupportedFiles() const override;
        std::span<const std::filesystem::path> getSupportedExtensions() const override;
        bool needsScan(const FileToScan& file) const override;
        std::unique_ptr<IFileScanOperation> createScanOperation(FileToScan&& fileToScan) const override;

        db::IDb& _db;
        ScannerSettings& _settings;
    };
} // namespace lms::scanner
