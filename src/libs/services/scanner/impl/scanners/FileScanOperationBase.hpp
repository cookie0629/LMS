#pragma once

#include <memory>
#include <vector>

#include "FileToScan.hpp"
#include "IFileScanOperation.hpp"

namespace lms::db
{
    class IDb;
} // namespace lms::db

namespace lms::scanner
{
    /**
     * @brief 扫描器设置（简化版）
     */
    struct ScannerSettings
    {
        // 暂时留空，后续可以添加配置项
    };

    /**
     * @brief 文件扫描操作基类
     */
    class FileScanOperationBase : public IFileScanOperation
    {
    public:
        FileScanOperationBase(FileToScan&& fileToScan, db::IDb& db, const ScannerSettings& settings);
        ~FileScanOperationBase() override = default;
        FileScanOperationBase(const FileScanOperationBase&) = delete;
        FileScanOperationBase& operator=(const FileScanOperationBase&) = delete;

    protected:
        const std::filesystem::path& getFilePath() const override { return _file.filePath; }
        const MediaLibraryInfo& getMediaLibrary() const { return _file.mediaLibrary; }
        db::IDb& getDb() { return _db; }
        const ScannerSettings& getScannerSettings() const { return _settings; }
        Wt::WDateTime getLastWriteTime() const { return _file.lastWriteTime; }
        std::size_t getFileSize() const { return _file.fileSize; }

        template<typename T, typename... CtrArgs>
        void addError(CtrArgs&&... args)
        {
            _errors.emplace_back(std::make_shared<T>(std::forward<CtrArgs>(args)...));
        }

        const ScanErrorVector& getErrors() override { return _errors; }

    private:
        const FileToScan _file;
        db::IDb& _db;
        const ScannerSettings& _settings;
        ScanErrorVector _errors;
    };
} // namespace lms::scanner

