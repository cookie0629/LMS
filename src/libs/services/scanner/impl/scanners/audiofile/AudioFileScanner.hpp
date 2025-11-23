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

    /**
     * @brief 音频文件扫描器（简化版）
     */
    class AudioFileScanner : public IFileScanner
    {
    public:
        AudioFileScanner(db::IDb& db, const ScannerSettings& settings);
        ~AudioFileScanner() override = default;
        AudioFileScanner(const AudioFileScanner&) = delete;
        AudioFileScanner& operator=(const AudioFileScanner&) = delete;

        // IFileScanner 接口方法（public）
        std::string_view getName() const override;
        std::span<const std::filesystem::path> getSupportedFiles() const override;
        std::span<const std::filesystem::path> getSupportedExtensions() const override;
        bool needsScan(const FileToScan& file) const override;
        std::unique_ptr<IFileScanOperation> createScanOperation(FileToScan&& fileToScan) const override;

    private:

        db::IDb& _db;
        const ScannerSettings& _settings;
        static const std::vector<std::filesystem::path> _supportedExtensions;
    };
} // namespace lms::scanner

