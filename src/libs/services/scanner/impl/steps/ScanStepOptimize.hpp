#pragma once

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    /**
     * @brief 数据库优化步骤（执行 PRAGMA optimize / ANALYZE）
     */
    class ScanStepOptimize final : public ScanStepBase
    {
    public:
        ScanStepOptimize(db::IDb& db, const ScannerSettings& settings);
        ~ScanStepOptimize() override = default;

        bool execute(ScanStats& stats) override;
        ScanStep getStep() const override { return ScanStep::Optimize; }
    };
} // namespace lms::scanner


