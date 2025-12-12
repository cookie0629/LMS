
#pragma once

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    class ScanStepComputeClusterStats : public ScanStepBase
    {
    public:
        using ScanStepBase::ScanStepBase;

    private:
        ScanStep getStep() const override { return ScanStep::ComputeClusterStats; }
        core::LiteralString getStepName() const override { return "Compute cluster stats"; }
        bool needProcess(const ScanContext& context) const override;
        void process(ScanContext& context) override;
    };
} // namespace lms::scanner
