
#pragma once

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    class ScanStepAssociateExternalLyrics : public ScanStepBase
    {
    public:
        using ScanStepBase::ScanStepBase;

    private:
        ScanStep getStep() const override { return ScanStep::AssociateExternalLyrics; }
        core::LiteralString getStepName() const override { return "Associate external lyrics"; }
        bool needProcess(const ScanContext& context) const override;
        void process(ScanContext& context) override;
    };
} // namespace lms::scanner
