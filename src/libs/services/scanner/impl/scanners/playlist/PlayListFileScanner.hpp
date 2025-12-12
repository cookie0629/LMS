
#pragma once

#include "scanners/IFileScanner.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::scanner
{
    struct ScannerSettings;

    class PlayListFileScanner : public IFileScanner
    {
    public:
        PlayListFileScanner(db::IDb& db, ScannerSettings& settings);
        ~PlayListFileScanner() override = default;
        PlayListFileScanner(const PlayListFileScanner&) = delete;
        PlayListFileScanner& operator=(const PlayListFileScanner&) = delete;

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
