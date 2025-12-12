
#pragma once

#include <deque>

#include "database/objects/PodcastId.hpp"

#include "RefreshStep.hpp"

namespace lms::podcast
{
    class DownloadPodcastArtworksStep : public RefreshStep
    {
        using RefreshStep::RefreshStep;

    private:
        core::LiteralString getName() const override;
        void run() override;

        void processNext();
        void process(db::PodcastId podcastId);

        std::deque<db::PodcastId> _podcastArtworksToDownload;
    };
} // namespace lms::podcast