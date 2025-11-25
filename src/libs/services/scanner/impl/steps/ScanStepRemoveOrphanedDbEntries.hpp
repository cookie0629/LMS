#pragma once

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    /**
     * @brief 删除孤立数据库条目的步骤
     */
    class ScanStepRemoveOrphanedDbEntries : public ScanStepBase
    {
    public:
        ScanStepRemoveOrphanedDbEntries(db::IDb& db, const ScannerSettings& settings);
        ~ScanStepRemoveOrphanedDbEntries() override = default;

        bool execute(ScanStats& stats) override;
        ScanStep getStep() const override { return ScanStep::RemoveOrphanedDbEntries; }
    };
} // namespace lms::scanner


