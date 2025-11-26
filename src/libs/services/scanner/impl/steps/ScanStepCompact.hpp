#pragma once

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    /**
     * @brief 数据库压缩步骤（执行 VACUUM）
     */
    class ScanStepCompact final : public ScanStepBase
    {
    public:
        ScanStepCompact(db::IDb& db, const ScannerSettings& settings);
        ~ScanStepCompact() override = default;

        bool execute(ScanStats& stats) override;
        ScanStep getStep() const override { return ScanStep::Compact; }
    };
} // namespace lms::scanner


