
#include "ScanStepRemoveOrphanedDbEntries.hpp"

#include "core/ILogger.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/objects/Artist.hpp"
#include "database/objects/Cluster.hpp"
#include "database/objects/Directory.hpp"
#include "database/objects/Medium.hpp"
#include "database/objects/Release.hpp"
#include "database/objects/Track.hpp"
#include "database/objects/TrackEmbeddedImage.hpp"

#include "ScanContext.hpp"

namespace lms::scanner
{
    bool ScanStepRemoveOrphanedDbEntries::needProcess([[maybe_unused]] const ScanContext& context) const
    {
        // fast enough when there is nothing to do
        return true;
    }

    void ScanStepRemoveOrphanedDbEntries::process(ScanContext& context)
    {
        removeOrphanedClusters(context);
        removeOrphanedClusterTypes(context);
        removeOrphanedArtists(context);
        removeOrphanedReleases(context);
        removeOrphanedMediums(context); // after release so that most entries are removed using the medium foreign key
        removeOrphanedReleaseTypes(context);
        removeOrphanedLabels(context);
        removeOrphanedCountries(context);
        removeOrphanedDirectories(context);
        removeOrphanedTrackEmbeddedImages(context);
    }

    void ScanStepRemoveOrphanedDbEntries::removeOrphanedClusters(ScanContext& context)
    {
        LMS_LOG(DBUPDATER, DEBUG, "Checking orphaned clusters...");
        removeOrphanedEntries<db::Cluster>(context);
    }

    void ScanStepRemoveOrphanedDbEntries::removeOrphanedClusterTypes(ScanContext& context)
    {
        LMS_LOG(DBUPDATER, DEBUG, "Checking orphaned cluster types...");
        removeOrphanedEntries<db::ClusterType>(context);
    }

    void ScanStepRemoveOrphanedDbEntries::removeOrphanedArtists(ScanContext& context)
    {
        LMS_LOG(DBUPDATER, DEBUG, "Checking orphaned artists...");
        removeOrphanedEntries<db::Artist>(context);
    }

    void ScanStepRemoveOrphanedDbEntries::removeOrphanedMediums(ScanContext& context)
    {
        LMS_LOG(DBUPDATER, DEBUG, "Checking orphaned mediums...");
        removeOrphanedEntries<db::Medium>(context);
    }

    void ScanStepRemoveOrphanedDbEntries::removeOrphanedReleases(ScanContext& context)
    {
        LMS_LOG(DBUPDATER, DEBUG, "Checking orphaned releases...");
        removeOrphanedEntries<db::Release>(context);
    }

    void ScanStepRemoveOrphanedDbEntries::removeOrphanedReleaseTypes(ScanContext& context)
    {
        LMS_LOG(DBUPDATER, DEBUG, "Checking orphaned release types...");
        removeOrphanedEntries<db::ReleaseType>(context);
    }

    void ScanStepRemoveOrphanedDbEntries::removeOrphanedLabels(ScanContext& context)
    {
        LMS_LOG(DBUPDATER, DEBUG, "Checking orphaned labels...");
        removeOrphanedEntries<db::Label>(context);
    }

    void ScanStepRemoveOrphanedDbEntries::removeOrphanedCountries(ScanContext& context)
    {
        LMS_LOG(DBUPDATER, DEBUG, "Checking orphaned countries...");
        removeOrphanedEntries<db::Country>(context);
    }

    void ScanStepRemoveOrphanedDbEntries::removeOrphanedDirectories(ScanContext& context)
    {
        LMS_LOG(DBUPDATER, DEBUG, "Checking orphaned directories...");
        removeOrphanedEntries<db::Directory>(context);
    }

    void ScanStepRemoveOrphanedDbEntries::removeOrphanedTrackEmbeddedImages(ScanContext& context)
    {
        LMS_LOG(DBUPDATER, DEBUG, "Checking orphaned embedded images in tracks...");
        removeOrphanedEntries<db::TrackEmbeddedImage>(context);
    }

    template<typename T>
    void ScanStepRemoveOrphanedDbEntries::removeOrphanedEntries(ScanContext& context)
    {
        constexpr std::size_t batchSize = 200;

        using IdType = typename T::IdType;

        db::Session& session{ _db.getTLSSession() };

        db::RangeResults<IdType> entries;
        while (!_abortScan)
        {
            {
                auto transaction{ session.createReadTransaction() };

                entries = T::findOrphanIds(session, db::Range{ 0, batchSize });
            };

            if (entries.results.empty())
                break;

            {
                auto transaction{ session.createWriteTransaction() };

                session.destroy<T>(entries.results);
            }

            context.currentStepStats.processedElems += entries.results.size();
            _progressCallback(context.currentStepStats);
        }
    }
} // namespace lms::scanner
