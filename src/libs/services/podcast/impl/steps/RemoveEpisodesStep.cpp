
#include "RemoveEpisodesStep.hpp"

#include <filesystem>
#include <vector>

#include "core/IConfig.hpp"
#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/objects/Artwork.hpp"
#include "database/objects/PodcastEpisode.hpp"

#include "Utils.hpp"

namespace lms::podcast
{
    core::LiteralString RemoveEpisodesStep::getName() const
    {
        return "Remove podcast episodes";
    }

    RemoveEpisodesStep::RemoveEpisodesStep(RefreshContext& context, OnDoneCallback callback)
        : RefreshStep{ context, std::move(callback) }
        , _autoDownloadEpisodesMaxAge{ core::Service<core::IConfig>::get()->getULong("podcast-auto-download-episodes-max-age-days", 30) }
    {
    }

    void RemoveEpisodesStep::run()
    {
        std::vector<db::PodcastEpisodeId> episodesToRemove;
        std::vector<db::ImageId> imagesToRemove;

        // Step 1 collect the episodes to remove
        {
            auto removePodcastFile{ [&](const db::PodcastEpisode::pointer& episode) {
                // We keep the artwork of the episode (TODO, not if the episode is no longer referenced by the podcast?)
                assert(!episode->getAudioRelativeFilePath().empty());
                utils::removeFile(getCachePath() / episode->getAudioRelativeFilePath());
                episodesToRemove.emplace_back(episode->getId());
            } };

            const Wt::WDateTime now{ Wt::WDateTime::currentDateTime() };

            auto& session{ getDb().getTLSSession() };
            auto transaction{ session.createReadTransaction() };

            db::PodcastEpisode::find(session, db::PodcastEpisode::FindParameters{}, [&](const db::PodcastEpisode::pointer& episode) {
                switch (episode->getManualDownloadState())
                {
                case db::PodcastEpisode::ManualDownloadState::None:
                    if (!episode->getAudioRelativeFilePath().empty() && now > episode->getPubDate().addDays(_autoDownloadEpisodesMaxAge.count())) // TODO make this configurable per podcast
                    {
                        LMS_LOG(PODCAST, INFO, "Removing episode '" << episode->getTitle() << "' because it is older than " << _autoDownloadEpisodesMaxAge.count() << " days");
                        removePodcastFile(episode);
                    }
                    break;

                case db::PodcastEpisode::ManualDownloadState::DownloadRequested:
                    // always keep the manually downloaded episodes
                    break;

                case db::PodcastEpisode::ManualDownloadState::DeleteRequested:
                    if (!episode->getAudioRelativeFilePath().empty())
                    {
                        LMS_LOG(PODCAST, DEBUG, "Removing episode '" << episode->getTitle() << "' because it was manually deleted");
                        removePodcastFile(episode);
                    }
                    break;
                }
            });
        }

        // second step, remove the database entries (must be consistent with first step!)
        if (!episodesToRemove.empty())
        {
            auto& session{ getDb().getTLSSession() };
            auto transaction{ session.createWriteTransaction() };

            for (db::PodcastEpisodeId episodeId : episodesToRemove)
            {
                if (db::PodcastEpisode::pointer episode{ db::PodcastEpisode::find(session, episodeId) })
                    episode.modify()->setAudioRelativeFilePath(std::filesystem::path{});
            }
        }

        onDone();
    }
} // namespace lms::podcast