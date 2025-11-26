#include "TrackMetadataParser.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <string_view>

#include "audio/ITagReader.hpp"
#include "core/String.hpp"

namespace lms::scanner
{
    TrackMetadata TrackMetadataParser::parseTrackMetadata(const audio::ITagReader& reader) const
    {
        TrackMetadata metadata;
        processTags(reader, metadata);
        return metadata;
    }

    void TrackMetadataParser::processTags(const audio::ITagReader& reader, TrackMetadata& metadata) const
    {
        metadata.title = getTagValue(reader, audio::TagType::Title);

        metadata.artists = extractValues(reader, audio::TagType::Artist, _params.artistTagDelimiters);
        if (!metadata.artists.empty())
        {
            metadata.artist = metadata.artists.front();
        }
        else
        {
            metadata.artist.reset();
        }

        metadata.album = getTagValue(reader, audio::TagType::Album);

        metadata.albumArtists = extractValues(reader, audio::TagType::AlbumArtist, _params.artistTagDelimiters);
        if (!metadata.albumArtists.empty())
        {
            metadata.albumArtist = metadata.albumArtists.front();
        }
        else
        {
            metadata.albumArtist.reset();
        }

        metadata.genres = extractValues(reader, audio::TagType::Genre, _params.defaultTagDelimiters);
        if (!metadata.genres.empty())
        {
            metadata.genre = metadata.genres.front();
        }
        else
        {
            metadata.genre.reset();
        }

        metadata.date = normalizeDate(getTagValue(reader, audio::TagType::Date));
        metadata.trackNumber = getTagValueAsInt(reader, audio::TagType::TrackNumber);
        metadata.discNumber = getTagValueAsInt(reader, audio::TagType::DiscNumber);
        metadata.comment = getTagValue(reader, audio::TagType::Comment);
    }

    std::vector<std::string> TrackMetadataParser::extractValues(const audio::ITagReader& reader, audio::TagType tagType, const std::vector<std::string>& delimiters) const
    {
        std::vector<std::string> rawValues = reader.getMultiTag(tagType);
        if (rawValues.empty())
        {
            if (auto single = reader.getTag(tagType))
            {
                rawValues.push_back(*single);
            }
        }

        std::vector<std::string> result;
        for (auto& raw : rawValues)
        {
            auto splitted = splitAndNormalizeValue(raw, delimiters);
            for (auto& value : splitted)
            {
                if (std::find(result.begin(), result.end(), value) == result.end())
                {
                    result.push_back(std::move(value));
                }
            }
        }
        return result;
    }

    std::vector<std::string> TrackMetadataParser::splitAndNormalizeValue(std::string value, const std::vector<std::string>& delimiters) const
    {
        if (value.empty())
        {
            return {};
        }

        std::vector<std::string> segments{ std::move(value) };
        for (const auto& delimiter : delimiters)
        {
            if (delimiter.empty())
            {
                continue;
            }

            std::vector<std::string> temp;
            temp.reserve(segments.size());

            for (const auto& segment : segments)
            {
                std::size_t start = 0;
                std::size_t pos = 0;
                while ((pos = segment.find(delimiter, start)) != std::string::npos)
                {
                    temp.emplace_back(segment.substr(start, pos - start));
                    start = pos + delimiter.size();
                }
                temp.emplace_back(segment.substr(start));
            }

            segments = std::move(temp);
        }

        std::vector<std::string> normalized;
        normalized.reserve(segments.size());
        for (const auto& segment : segments)
        {
            auto trimmed = core::stringUtils::stringTrim(segment);
            if (!trimmed.empty())
            {
                normalized.emplace_back(trimmed);
            }
        }

        return normalized;
    }

    std::optional<std::string> TrackMetadataParser::getTagValue(const audio::ITagReader& reader, audio::TagType tagType) const
    {
        auto value = reader.getTag(tagType);
        if (!value)
            return std::nullopt;

        // 修剪字符串
        std::string_view trimmed = core::stringUtils::stringTrim(*value);
        if (trimmed.empty())
            return std::nullopt;

        return std::string{ trimmed };
    }

    std::optional<int> TrackMetadataParser::getTagValueAsInt(const audio::ITagReader& reader, audio::TagType tagType) const
    {
        auto value = getTagValue(reader, tagType);
        return parseNumberValue(value);
    }

    std::optional<int> TrackMetadataParser::parseNumberValue(const std::optional<std::string>& value) const
    {
        if (!value)
        {
            return std::nullopt;
        }

        auto trimmed = core::stringUtils::stringTrim(*value);
        if (trimmed.empty())
        {
            return std::nullopt;
        }

        std::string digits;
        digits.reserve(trimmed.size());
        bool capturing = false;

        for (char ch : trimmed)
        {
            if (std::isdigit(static_cast<unsigned char>(ch)))
            {
                digits.push_back(ch);
                capturing = true;
            }
            else if (capturing)
            {
                break;
            }
        }

        if (digits.empty())
        {
            return std::nullopt;
        }

        try
        {
            return std::stoi(digits);
        }
        catch (const std::exception&)
        {
            return std::nullopt;
        }
    }

    std::optional<std::string> TrackMetadataParser::normalizeDate(const std::optional<std::string>& value) const
    {
        if (!value)
        {
            return std::nullopt;
        }

        std::string trimmed{ core::stringUtils::stringTrim(*value) };
        if (trimmed.empty())
        {
            return std::nullopt;
        }

        for (std::size_t i = 0; i + 4 <= trimmed.size(); ++i)
        {
            bool digitsOnly = true;
            for (std::size_t j = 0; j < 4; ++j)
            {
                if (!std::isdigit(static_cast<unsigned char>(trimmed[i + j])))
                {
                    digitsOnly = false;
                    break;
                }
            }

            if (digitsOnly)
            {
                return trimmed.substr(i, 4);
            }
        }

        const bool allDigits = std::all_of(trimmed.begin(), trimmed.end(), [](unsigned char c) {
            return std::isdigit(c);
        });

        if (allDigits)
        {
            if (trimmed.size() > 4)
            {
                trimmed.resize(4);
            }
            return trimmed;
        }

        return trimmed;
    }
} // namespace lms::scanner

