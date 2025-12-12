
#pragma once

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    class ScanStepCompact : public ScanStepBase
    {
    public:
        using ScanStepBase::ScanStepBase;

    private:
        ScanStep getStep() const override { return ScanStep::Compact; }
        core::LiteralString getStepName() const override { return "Compact"; }
        bool needProcess(const ScanContext& context) const override;
        void process(ScanContext& context) override;
    };
} // namespace lms::scanner
