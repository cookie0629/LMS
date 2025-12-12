
#pragma once

#include <filesystem>
#include <memory>

#include <boost/asio/io_context.hpp>

#include "database/objects/PodcastEpisodeId.hpp"
#include "database/objects/PodcastId.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::podcast
{
    class IPodcastService
    {
    public:
        virtual ~IPodcastService() = default;

        virtual std::filesystem::path getCachePath() const = 0;

        virtual db::PodcastId addPodcast(std::string_view url) = 0;
        virtual bool removePodcast(db::PodcastId podcast) = 0;
        virtual void refreshPodcasts() = 0;

        virtual bool downloadPodcastEpisode(db::PodcastEpisodeId episode) = 0;
        virtual bool deletePodcastEpisode(db::PodcastEpisodeId episode) = 0;
    };

    std::unique_ptr<IPodcastService> createPodcastService(boost::asio::io_context& ioContext, db::IDb& db, const std::filesystem::path& cachePath);
} // namespace lms::podcast
