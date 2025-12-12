
#pragma once

#include <deque>

#include "database/objects/PodcastEpisodeId.hpp"

#include "RefreshStep.hpp"

namespace lms::podcast
{
    class DownloadEpisodesStep : public RefreshStep
    {
    public:
        DownloadEpisodesStep(RefreshContext& context, OnDoneCallback callback);

    private:
        core::LiteralString getName() const override;
        void run() override;

        void collectEpisodes();

        void processNext();
        void process(db::PodcastEpisodeId episodeId);

        const bool _autoDownloadEpisodes;
        const std::chrono::days _autoDownloadEpisodesMaxAge;

        std::deque<db::PodcastEpisodeId> _episodesToDownload;
    };

} // namespace lms::podcast