#pragma once

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    /**
     * @brief 检查已删除的文件步骤
     */
    class ScanStepCheckForRemovedFiles : public ScanStepBase
    {
    public:
        ScanStepCheckForRemovedFiles(db::IDb& db, const ScannerSettings& settings);
        ~ScanStepCheckForRemovedFiles() override = default;

        bool execute(ScanStats& stats) override;
        ScanStep getStep() const override { return ScanStep::CheckForRemovedFiles; }
    };
} // namespace lms::scanner

