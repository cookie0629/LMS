
#pragma once

#include <deque>

#include "database/objects/PodcastEpisodeId.hpp"

#include "RefreshStep.hpp"

namespace lms::podcast
{
    class DownloadEpisodeArtworksStep : public RefreshStep
    {
        using RefreshStep::RefreshStep;

    private:
        core::LiteralString getName() const override;
        void run() override;

        void processNext();
        void process(db::PodcastEpisodeId episodeId);

        std::deque<db::PodcastEpisodeId> _episodeArtworksToDownload;
    };
} // namespace lms::podcast