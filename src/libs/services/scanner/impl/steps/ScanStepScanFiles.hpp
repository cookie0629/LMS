#pragma once

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    /**
     * @brief 扫描文件步骤（简化版）
     */
    class ScanStepScanFiles : public ScanStepBase
    {
    public:
        ScanStepScanFiles(db::IDb& db, const ScannerSettings& settings);
        ~ScanStepScanFiles() override = default;

        bool execute(ScanStats& stats) override;
        ScanStep getStep() const override { return ScanStep::ScanFiles; }

    private:
        // TODO: 实现文件扫描逻辑
    };
} // namespace lms::scanner

