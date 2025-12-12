
#pragma once

#include <string>
#include <vector>

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    class ScanStepAssociateArtistImages : public ScanStepBase
    {
    public:
        ScanStepAssociateArtistImages(InitParams& initParams);
        ~ScanStepAssociateArtistImages() override = default;
        ScanStepAssociateArtistImages(const ScanStepAssociateArtistImages&) = delete;
        ScanStepAssociateArtistImages& operator=(const ScanStepAssociateArtistImages&) = delete;

    private:
        ScanStep getStep() const override { return ScanStep::AssociateArtistImages; }
        core::LiteralString getStepName() const override { return "Associate artist images"; }
        bool needProcess(const ScanContext& context) const override;
        void process(ScanContext& context) override;

        const std::vector<std::string> _artistFileNames;
        const std::vector<std::string> _artistInfoFileNames;
    };
} // namespace lms::scanner
