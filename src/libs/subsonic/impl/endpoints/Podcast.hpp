
#pragma once

#include "RequestContext.hpp"
#include "SubsonicResponse.hpp"

namespace lms::api::subsonic
{
    Response handleGetPodcasts(RequestContext& context);
    Response handleGetNewestPodcasts(RequestContext& context);
    Response handleRefreshPodcasts(RequestContext& context);
    Response handleCreatePodcastChannel(RequestContext& context);
    Response handleDeletePodcastChannel(RequestContext& context);
    Response handleDeletePodcastEpisode(RequestContext& context);
    Response handleDownloadPodcastEpisode(RequestContext& context);
    Response handleGetPodcastEpisode(RequestContext& context);
} // namespace lms::api::subsonic
