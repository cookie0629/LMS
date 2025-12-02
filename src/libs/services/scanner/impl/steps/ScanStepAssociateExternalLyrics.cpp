#include "ScanStepAssociateExternalLyrics.hpp"

#include <deque>
#include <filesystem>

#include "core/ILogger.hpp"

#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/Track.hpp"
#include "database/objects/TrackLyrics.hpp"

namespace lms::scanner
{
    namespace
    {
        struct TrackLyricsAssociation
        {
            db::TrackLyricsId trackLyricsId;
            db::TrackId trackId;
        };

        using TrackLyricsAssociationContainer = std::deque<TrackLyricsAssociation>;

        db::Track::pointer getMatchingTrack(db::Session& session, const db::TrackLyrics::pointer& lyrics)
        {
            db::Track::pointer matchingTrack;

            const std::filesystem::path lyricsDir{ lyrics->getAbsoluteFilePath().parent_path() };
            if (lyricsDir.empty())
            {
                return {};
            }

            auto tryMatch = [&](std::string_view stem) {
                db::Track::find(session, [&](const db::Track::pointer& track) {
                    const auto trackPath = track->getAbsoluteFilePath();

                    if (trackPath.parent_path() != lyricsDir)
                    {
                        return;
                    }

                    if (trackPath.filename().stem().string() != stem)
                    {
                        return;
                    }

                    if (matchingTrack)
                    {
                        LMS_LOG(SCANNER, DEBUG, "External lyrics '" << lyrics->getAbsoluteFilePath()
                                                                     << "' already matched with '" << matchingTrack->getAbsoluteFilePath()
                                                                     << "', replaced by '" << track->getAbsoluteFilePath() << "'");
                    }

                    matchingTrack = track;
                });
            };

            // 先用歌词文件自身的 stem 匹配；如果失败，再尝试去掉语言码（file.language.lrc）
            tryMatch(lyrics->getFileStem());
            if (!matchingTrack)
            {
                std::filesystem::path stem{ lyrics->getFileStem() };
                if (stem.has_extension())
                {
                    tryMatch(stem.stem().string());
                }
            }

            return matchingTrack;
        }

        void collectAssociations(db::Session& session, TrackLyricsAssociationContainer& trackLyricsAssociations, std::size_t& processedCount)
        {
            auto transaction{ session.createReadTransaction() };

            db::TrackLyrics::FindParameters params;
            params.external = true;

            db::TrackLyrics::find(session, params, [&](const db::TrackLyrics::pointer& trackLyrics) {
                db::Track::pointer track{ getMatchingTrack(session, trackLyrics) };
                if (track != trackLyrics->getTrack())
                {
                    LMS_LOG(SCANNER, DEBUG, "Updating track for external lyrics " << trackLyrics->getAbsoluteFilePath()
                                                                                   << ", using " << (track ? track->getAbsoluteFilePath() : "<none>"));
                    trackLyricsAssociations.push_back(
                        TrackLyricsAssociation{ .trackLyricsId = trackLyrics->getId(),
                                                .trackId = (track ? track->getId() : db::TrackId{}) });
                }
                else if (!track)
                {
                    LMS_LOG(SCANNER, DEBUG, "No track found for external lyrics " << trackLyrics->getAbsoluteFilePath() << "'");
                }

                ++processedCount;
            });
        }

        void updateTrackLyrics(db::Session& session, const TrackLyricsAssociation& trackLyricsAssociation)
        {
            db::TrackLyrics::pointer lyrics{ db::TrackLyrics::find(session, trackLyricsAssociation.trackLyricsId) };
            if (!lyrics)
            {
                return;
            }

            db::Track::pointer track;
            if (trackLyricsAssociation.trackId.getValue() != 0)
            {
                track = db::Track::find(session, trackLyricsAssociation.trackId);
            }

            lyrics.modify()->setTrack(track);
        }

        void updateTrackLyrics(db::Session& session, TrackLyricsAssociationContainer& lyricsAssociations, std::size_t& updatedCount)
        {
            constexpr std::size_t writeBatchSize{ 20 };

            while (!lyricsAssociations.empty())
            {
                auto transaction{ session.createWriteTransaction() };

                for (std::size_t i{}; !lyricsAssociations.empty() && i < writeBatchSize; ++i)
                {
                    updateTrackLyrics(session, lyricsAssociations.front());
                    lyricsAssociations.pop_front();
                    ++updatedCount;
                }

                transaction.commit();
            }
        }
    } // namespace

    ScanStepAssociateExternalLyrics::ScanStepAssociateExternalLyrics(db::IDb& db, const ScannerSettings& settings)
        : ScanStepBase{ db, settings }
    {
    }

    bool ScanStepAssociateExternalLyrics::execute(ScanStats& stats)
    {
        LMS_LOG(SCANNER, INFO, "Starting associate external lyrics step");

        try
        {
            db::Session& session{ getDb().getTLSSession() };

            TrackLyricsAssociationContainer trackLyricsAssociations;
            std::size_t processedCount{};
            std::size_t updatedCount{};

            collectAssociations(session, trackLyricsAssociations, processedCount);
            updateTrackLyrics(session, trackLyricsAssociations, updatedCount);

            stats.updates += updatedCount;

            LMS_LOG(SCANNER, INFO, "Associate external lyrics completed, processed "
                                       << processedCount << " external lyrics, updated " << updatedCount << " entries");
        }
        catch (const std::exception& e)
        {
            LMS_LOG(SCANNER, ERROR, "Associate external lyrics step failed: " << e.what());
            return false;
        }

        return true;
    }
} // namespace lms::scanner



