
#pragma once

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    class ScanStepRemoveOrphanedDbEntries : public ScanStepBase
    {
    public:
        using ScanStepBase::ScanStepBase;

    private:
        core::LiteralString getStepName() const override { return "Remove orphaned DB entries"; }
        ScanStep getStep() const override { return ScanStep::RemoveOrphanedDbEntries; }
        bool needProcess(const ScanContext& context) const override;
        void process(ScanContext& context) override;

        void removeOrphanedClusters(ScanContext& context);
        void removeOrphanedClusterTypes(ScanContext& context);
        void removeOrphanedArtists(ScanContext& context);
        void removeOrphanedMediums(ScanContext& context);
        void removeOrphanedReleases(ScanContext& context);
        void removeOrphanedReleaseTypes(ScanContext& context);
        void removeOrphanedLabels(ScanContext& context);
        void removeOrphanedCountries(ScanContext& context);
        void removeOrphanedDirectories(ScanContext& context);
        void removeOrphanedTrackEmbeddedImages(ScanContext& context);

        template<typename T>
        void removeOrphanedEntries(ScanContext& context);
    };
} // namespace lms::scanner
