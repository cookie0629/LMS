
#pragma once

#include "IFileScanner.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::scanner
{
    struct ScannerSettings;

    class ImageFileScanner : public IFileScanner
    {
    public:
        ImageFileScanner(db::IDb& db, ScannerSettings& settings);
        ~ImageFileScanner() override = default;
        ImageFileScanner(const ImageFileScanner&) = delete;
        ImageFileScanner& operator=(const ImageFileScanner&) = delete;

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
