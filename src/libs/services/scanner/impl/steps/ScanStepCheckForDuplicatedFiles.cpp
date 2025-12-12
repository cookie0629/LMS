
#include "ScanStepCheckForDuplicatedFiles.hpp"

#include "core/ILogger.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/objects/Track.hpp"

#include "ScanContext.hpp"

namespace lms::scanner
{
    bool ScanStepCheckForDuplicatedFiles::needProcess([[maybe_unused]] const ScanContext& context) const
    {
        return true;
    }

    void ScanStepCheckForDuplicatedFiles::process(ScanContext& context)
    {
        using namespace db;

        Session& session{ _db.getTLSSession() };
        auto transaction{ session.createReadTransaction() };

        const RangeResults<TrackId> tracks = Track::findIdsTrackMBIDDuplicates(session);
        for (const TrackId trackId : tracks.results)
        {
            if (_abortScan)
                break;

            const Track::pointer track{ Track::find(session, trackId) };
            if (auto trackMBID{ track->getTrackMBID() })
            {
                LMS_LOG(DBUPDATER, INFO, "Found duplicated track MBID [" << trackMBID->getAsString() << "], file: " << track->getAbsoluteFilePath().string() << " - " << track->getName());
                context.stats.duplicates.emplace_back(ScanDuplicate{ track->getId(), DuplicateReason::SameTrackMBID });
                context.currentStepStats.processedElems++;
                _progressCallback(context.currentStepStats);
            }
        }

        LMS_LOG(DBUPDATER, DEBUG, "Found " << context.currentStepStats.processedElems << " duplicated audio files");
    }
} // namespace lms::scanner
