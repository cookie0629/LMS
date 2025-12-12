
#pragma once

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    class ScanStepArtistReconciliation : public ScanStepBase
    {
    public:
        using ScanStepBase::ScanStepBase;

    private:
        ScanStep getStep() const override { return ScanStep::ReconciliateArtists; }
        core::LiteralString getStepName() const override { return "Artist reconciliation"; }
        bool needProcess(const ScanContext& context) const override;
        void process(ScanContext& context) override;

        void updateArtistPreferredName(ScanContext& context);

        void updateLinksForArtistNameNoLongerMatch(ScanContext& context);
        void updateLinksWithArtistNameAmbiguity(ScanContext& context);
        void updateArtistInfoForArtistNameNoLongerMatch(ScanContext& context);
        void updateArtistInfoWithArtistNameAmbiguity(ScanContext& context);
    };
} // namespace lms::scanner
