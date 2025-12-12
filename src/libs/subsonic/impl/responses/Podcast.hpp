
#pragma once

#include "database/Object.hpp"

#include "SubsonicResponse.hpp"

namespace lms::db
{
    class Podcast;
    class PodcastEpisode;
} // namespace lms::db

namespace lms::api::subsonic
{
    struct RequestContext;

    Response::Node createPodcastEpisodeNode(const db::ObjectPtr<db::PodcastEpisode>& episode);
    Response::Node createPodcastNode(RequestContext& context, const db::ObjectPtr<db::Podcast>& podcast, bool includeEpisodes);
} // namespace lms::api::subsonic