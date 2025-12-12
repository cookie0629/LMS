
#pragma once

#include "scanners/IFileScanner.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::scanner
{
    struct ScannerSettings;

    class ArtistInfoFileScanner : public IFileScanner
    {
    public:
        ArtistInfoFileScanner(db::IDb& db, const ScannerSettings& _settings);
        ~ArtistInfoFileScanner() override = default;
        ArtistInfoFileScanner(const ArtistInfoFileScanner&) = delete;
        ArtistInfoFileScanner& operator=(const ArtistInfoFileScanner&) = delete;

    private:
        core::LiteralString getName() const override;
        std::span<const std::filesystem::path> getSupportedFiles() const override;
        std::span<const std::filesystem::path> getSupportedExtensions() const override;
        bool needsScan(const FileToScan& file) const override;
        std::unique_ptr<IFileScanOperation> createScanOperation(FileToScan&& fileToScan) const override;

        db::IDb& _db;
        const ScannerSettings& _settings;
    };
} // namespace lms::scanner