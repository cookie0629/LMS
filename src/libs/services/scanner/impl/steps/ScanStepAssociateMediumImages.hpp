
#pragma once

#include <string>
#include <vector>

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    class ScanStepAssociateMediumImages : public ScanStepBase
    {
    public:
        ScanStepAssociateMediumImages(InitParams& initParams);
        ~ScanStepAssociateMediumImages() override = default;
        ScanStepAssociateMediumImages(const ScanStepAssociateMediumImages&) = delete;
        ScanStepAssociateMediumImages& operator=(const ScanStepAssociateMediumImages&) = delete;

    private:
        ScanStep getStep() const override { return ScanStep::AssociateArtistImages; }
        core::LiteralString getStepName() const override { return "Associate medium images"; }
        bool needProcess(const ScanContext& context) const override;
        void process(ScanContext& context) override;

        const std::vector<std::string> _mediumFileNames;
    };
} // namespace lms::scanner
