
#pragma once

#include <functional>

#include "IScanStep.hpp"
#include "ScanErrorLogger.hpp"

namespace lms::core
{
    class IJobScheduler;
}

namespace lms::db
{
    class IDb;
}

namespace lms::scanner
{
    class FileScanners;
    struct ScannerSettings;
    struct ScanStepStats;
    struct ScanContext;

    // ScanStepBase: 所有扫描步骤的公共基类，封装 Job 调度、错误记录以及文件扫描器访问。
    // ScanStepBase: базовый класс для всех шагов сканера, инкапсулирует планировщик задач, логгер ошибок и доступ к FileScanners.
    class ScanStepBase : public IScanStep
    {
    public:
        using ProgressCallback = std::function<void(const ScanStepStats& stats)>;

        struct InitParams
        {
            core::IJobScheduler& jobScheduler;
            const ScannerSettings& settings;
            const ScannerSettings* lastScanSettings{};
            ProgressCallback progressCallback;
            bool& abortScan;
            db::IDb& db;
            const FileScanners& fileScanners;
            const std::filesystem::path& cachePath;
        };
        ScanStepBase(InitParams& initParams);
        ~ScanStepBase() override;
        ScanStepBase(const ScanStepBase&) = delete;
        ScanStepBase& operator=(const ScanStepBase&) = delete;

    protected:
        core::IJobScheduler& getJobScheduler() { return _jobScheduler; };
        const ScannerSettings* getLastScanSettings() const { return _lastScanSettings; }
        const FileScanners& getFileScanners() const { return _fileScanners; }
        const std::filesystem::path& getCachePath() const { return _cachePath; }

        void addError(ScanContext& context, std::shared_ptr<ScanError> error);

        template<typename T, typename... CtrArgs>
        void addError(ScanContext& context, CtrArgs&&... args)
        {
            auto error{ std::make_shared<T>(std::forward<CtrArgs>(args)...) };
            addError(context, error);
        }

        const ScannerSettings& _settings;
        ProgressCallback _progressCallback;
        bool& _abortScan;
        db::IDb& _db;

    private:
        core::IJobScheduler& _jobScheduler;
        const FileScanners& _fileScanners;
        const std::filesystem::path& _cachePath;

        const ScannerSettings* _lastScanSettings{};
        ScanErrorLogger _scanErrorLogger;
    };
} // namespace lms::scanner
