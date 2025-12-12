
#pragma once

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    class ScanStepCheckForDuplicatedFiles : public ScanStepBase
    {
    public:
        using ScanStepBase::ScanStepBase;

    private:
        core::LiteralString getStepName() const override { return "Check for duplicated files"; }
        ScanStep getStep() const override { return ScanStep::CheckForDuplicatedFiles; }
        bool needProcess(const ScanContext& context) const override;
        void process(ScanContext& context) override;
    };
} // namespace lms::scanner
