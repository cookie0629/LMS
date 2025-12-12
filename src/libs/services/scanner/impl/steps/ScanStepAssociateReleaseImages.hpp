
#pragma once

#include <string>
#include <vector>

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    class ScanStepAssociateReleaseImages : public ScanStepBase
    {
    public:
        ScanStepAssociateReleaseImages(InitParams& initParams);
        ~ScanStepAssociateReleaseImages() override = default;
        ScanStepAssociateReleaseImages(const ScanStepAssociateReleaseImages&) = delete;
        ScanStepAssociateReleaseImages& operator=(const ScanStepAssociateReleaseImages&) = delete;

    private:
        ScanStep getStep() const override { return ScanStep::AssociateReleaseImages; }
        core::LiteralString getStepName() const override { return "Associate release images"; }
        bool needProcess(const ScanContext& context) const override;
        void process(ScanContext& context) override;

        const std::vector<std::string> _releaseImageFileNames;
    };
} // namespace lms::scanner
