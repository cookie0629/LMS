
#pragma once

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    class ScanStepAssociateTrackImages : public ScanStepBase
    {
    public:
        ScanStepAssociateTrackImages(InitParams& initParams);
        ~ScanStepAssociateTrackImages() override = default;
        ScanStepAssociateTrackImages(const ScanStepAssociateTrackImages&) = delete;
        ScanStepAssociateTrackImages& operator=(const ScanStepAssociateTrackImages&) = delete;

    private:
        ScanStep getStep() const override { return ScanStep::AssociateTrackImages; }
        core::LiteralString getStepName() const override { return "Associate track images"; }
        bool needProcess(const ScanContext& context) const override;
        void process(ScanContext& context) override;
    };
} // namespace lms::scanner
