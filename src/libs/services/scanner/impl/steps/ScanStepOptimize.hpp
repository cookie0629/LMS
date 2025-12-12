
#pragma once

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    class ScanStepOptimize : public ScanStepBase
    {
    public:
        using ScanStepBase::ScanStepBase;

    private:
        ScanStep getStep() const override { return ScanStep::Optimize; }
        core::LiteralString getStepName() const override { return "Optimize"; }
        bool needProcess(const ScanContext& context) const override;
        void process(ScanContext& context) override;
    };
} // namespace lms::scanner
