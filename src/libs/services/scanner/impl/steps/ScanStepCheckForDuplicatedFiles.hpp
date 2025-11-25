#pragma once

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    /**
     * @brief 检查重复文件步骤
     */
    class ScanStepCheckForDuplicatedFiles : public ScanStepBase
    {
    public:
        ScanStepCheckForDuplicatedFiles(db::IDb& db, const ScannerSettings& settings);
        ~ScanStepCheckForDuplicatedFiles() override = default;

        bool execute(ScanStats& stats) override;
        ScanStep getStep() const override { return ScanStep::CheckForDuplicatedFiles; }
    };
} // namespace lms::scanner


