
#pragma once

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    class ScanStepAssociatePlayListTracks : public ScanStepBase
    {
    public:
        using ScanStepBase::ScanStepBase;

    private:
        ScanStep getStep() const override { return ScanStep::AssociatePlayListTracks; }
        core::LiteralString getStepName() const override { return "Associate playlist tracks"; }
        bool needProcess(const ScanContext& context) const override;
        void process(ScanContext& context) override;
    };
} // namespace lms::scanner
