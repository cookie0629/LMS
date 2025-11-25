#pragma once

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    /**
     * @brief 更新媒体库字段（确保目录与媒体库关联一致）
     */
    class ScanStepUpdateLibraryFields : public ScanStepBase
    {
    public:
        ScanStepUpdateLibraryFields(db::IDb& db, const ScannerSettings& settings);
        ~ScanStepUpdateLibraryFields() override = default;

        bool execute(ScanStats& stats) override;
        ScanStep getStep() const override { return ScanStep::UpdateLibraryFields; }
    };
} // namespace lms::scanner


