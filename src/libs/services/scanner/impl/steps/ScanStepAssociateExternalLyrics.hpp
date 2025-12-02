#pragma once

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    /**
     * @brief 将外部歌词文件关联到对应的 Track（根据同一目录下的文件名匹配）
     */
    class ScanStepAssociateExternalLyrics : public ScanStepBase
    {
    public:
        ScanStepAssociateExternalLyrics(db::IDb& db, const ScannerSettings& settings);
        ~ScanStepAssociateExternalLyrics() override = default;

        bool execute(ScanStats& stats) override;
        ScanStep getStep() const override { return ScanStep::AssociateExternalLyrics; }
    };
} // namespace lms::scanner


