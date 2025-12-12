
#pragma once

#include <string_view>

#include "Exception.hpp"

#include "PodcastTypes.hpp"

namespace lms::podcast
{
    class ParseException : public Exception
    {
    public:
        using Exception::Exception;
    };

    Podcast parsePodcastRssFeed(std::string_view rssXml);
} // namespace lms::podcast