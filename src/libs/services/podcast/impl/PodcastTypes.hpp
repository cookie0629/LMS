
#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <vector>

#include <Wt/WDateTime.h>

namespace lms::podcast
{

    struct EnclosureUrl
    {
        std::string url;
        std::size_t length;
        std::string type;
    };

    struct PodcastEpisode
    {
        std::string url;
        std::string title;
        std::string link;
        std::string description;
        Wt::WDateTime pubDate;
        std::string author;
        std::string category;
        std::optional<bool> explicitContent;
        std::string imageUrl;
        std::string ownerEmail;
        std::string guid;
        EnclosureUrl enclosureUrl;
        std::chrono::milliseconds duration{ 0 };
    };

    struct Podcast
    {
        std::string title;
        std::string link;
        std::string description;
        std::string language;
        std::string copyright;
        Wt::WDateTime lastBuildDate;
        // itunes fields
        std::string newUrl;
        std::string author;
        std::string category;
        std::optional<bool> explicitContent;
        std::string imageUrl;
        std::string ownerEmail;
        std::string ownerName;
        std::string subtitle;
        std::string summary;

        std::vector<PodcastEpisode> episodes; // List of episodes in the podcast
    };

} // namespace lms::podcast