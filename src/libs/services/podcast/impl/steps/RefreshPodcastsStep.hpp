
#pragma once

#include <queue>

#include "database/objects/PodcastId.hpp"

#include "RefreshStep.hpp"

namespace lms::podcast
{
    class RefreshPodcastsStep : public RefreshStep
    {
        using RefreshStep::RefreshStep;

    private:
        core::LiteralString getName() const override;
        void run() override;

        void refreshNextPodcast();
        void refreshPodcast(db::PodcastId podcastId);

        std::queue<db::PodcastId> podcastsToRefresh;
    };

} // namespace lms::podcast