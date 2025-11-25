#include "ScanStepCheckForDuplicatedFiles.hpp"

#include <unordered_map>
#include <vector>

#include "core/ILogger.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/Track.hpp"

namespace lms::scanner
{
    namespace
    {
        using TrackPointerVector = std::vector<db::Track::pointer>;

        void accumulateDuplicates(const TrackPointerVector& candidates,
                                  ScanStats& stats,
                                  DuplicateReason reason)
        {
            if (candidates.size() < 2)
            {
                return;
            }

            for (const auto& track : candidates)
            {
                if (!track)
                {
                    continue;
                }

                const auto trackId = track->getId();
                stats.duplicates.push_back(ScanDuplicate{
                    .trackId = static_cast<std::int64_t>(trackId.getValue()),
                    .reason = reason,
                });
            }
        }
    } // namespace

    ScanStepCheckForDuplicatedFiles::ScanStepCheckForDuplicatedFiles(db::IDb& db, const ScannerSettings& settings)
        : ScanStepBase{ db, settings }
    {
    }

    bool ScanStepCheckForDuplicatedFiles::execute(ScanStats& stats)
    {
        LMS_LOG(SCANNER, INFO, "Starting duplicate files check");

        try
        {
            auto& session = getDb().getTLSSession();

            // 读取所有 Track
            std::vector<db::Track::pointer> tracks;
            {
                auto readTx = session.createReadTransaction();
                db::Track::find(session, [&tracks](const db::Track::pointer& track) {
                    tracks.push_back(track);
                });
                readTx.commit();
            }

            LMS_LOG(SCANNER, INFO, "Loaded " << tracks.size() << " tracks for duplicate analysis");

            std::unordered_map<std::string, TrackPointerVector> tracksByPath;
            std::unordered_map<long long, TrackPointerVector> tracksBySize;

            tracksByPath.reserve(tracks.size());
            tracksBySize.reserve(tracks.size());

            for (const auto& track : tracks)
            {
                if (!track)
                {
                    continue;
                }

                tracksByPath[track->getAbsoluteFilePath().string()].push_back(track);
                tracksBySize[track->getFileSize()].push_back(track);
            }

            const auto initialDuplicatesCount = stats.duplicates.size();

            for (const auto& [path, candidates] : tracksByPath)
            {
                (void)path;
                accumulateDuplicates(candidates, stats, DuplicateReason::SameHash);
            }

            for (const auto& [size, candidates] : tracksBySize)
            {
                (void)size;
                accumulateDuplicates(candidates, stats, DuplicateReason::SameTrackMBID);
            }

            const auto addedDuplicates = stats.duplicates.size() - initialDuplicatesCount;
            LMS_LOG(SCANNER, INFO, "Duplicate files check completed: " << addedDuplicates << " duplicates detected");
        }
        catch (const std::exception& e)
        {
            LMS_LOG(SCANNER, ERROR, "Duplicate files check failed: " << e.what());
            return false;
        }

        return true;
    }
} // namespace lms::scanner


