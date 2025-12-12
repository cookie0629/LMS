
#pragma once

#include <set>
#include <string>

#include "audio/IAudioFileInfo.hpp"
#include "audio/ITagReader.hpp"

#include "types/TrackMetadata.hpp"

namespace lms::scanner
{
    class TrackMetadataParser
    {
    public:
        struct SortByLengthDesc
        {
            bool operator()(const std::string& a, const std::string& b) const
            {
                if (a.length() != b.length())
                    return a.length() > b.length();
                return a < b; // Break ties using lexicographical order
            }
        };

        using WhiteList = std::set<std::string, SortByLengthDesc>;
        struct Parameters
        {
            std::vector<std::string> artistTagDelimiters;
            WhiteList artistsToNotSplit;
            std::vector<std::string> defaultTagDelimiters;
            std::vector<std::string> userExtraTags;
        };

        TrackMetadataParser(const Parameters& params = {});
        ~TrackMetadataParser();
        TrackMetadataParser(const TrackMetadataParser&) = delete;
        TrackMetadataParser& operator=(const TrackMetadataParser&) = delete;

        Track parseTrackMetaData(const audio::ITagReader& reader) const;

    private:
        void processTags(const audio::ITagReader& reader, Track& track) const;

        std::optional<Medium> getMedium(const audio::ITagReader& tagReader) const;
        std::optional<Release> getRelease(const audio::ITagReader& tagReader) const;

        const Parameters _params;
    };
} // namespace lms::scanner
