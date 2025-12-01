#pragma once

#include <memory>
#include <span>
#include <vector>

#include "../IFileScanner.hpp"

namespace lms::db
{
    class IDb;
} // namespace lms::db

namespace lms::scanner
{
    struct ScannerSettings;

    class LyricsFileScanner : public IFileScanner
    {
    public:
        LyricsFileScanner(db::IDb& db, const ScannerSettings& settings);
        ~LyricsFileScanner() override = default;
        LyricsFileScanner(const LyricsFileScanner&) = delete;
        LyricsFileScanner& operator=(const LyricsFileScanner&) = delete;

        std::string_view getName() const override;
        std::span<const std::filesystem::path> getSupportedFiles() const override;
        std::span<const std::filesystem::path> getSupportedExtensions() const override;
        bool needsScan(const FileToScan& file) const override;
        std::unique_ptr<IFileScanOperation> createScanOperation(FileToScan&& fileToScan) const override;

    private:
        db::IDb& _db;
        const ScannerSettings& _settings;
    };
} // namespace lms::scanner


