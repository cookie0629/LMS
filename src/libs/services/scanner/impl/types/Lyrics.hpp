
#pragma once

#include <chrono>
#include <map>
#include <string>
#include <vector>

namespace lms::scanner
{
    struct Lyrics
    {
        std::string language;
        std::chrono::milliseconds offset{};
        std::string displayArtist;
        std::string displayAlbum;
        std::string displayTitle;

        std::map<std::chrono::milliseconds, std::string> synchronizedLines;
        std::vector<std::string> unsynchronizedLines;
    };
} // namespace lms::scanner
