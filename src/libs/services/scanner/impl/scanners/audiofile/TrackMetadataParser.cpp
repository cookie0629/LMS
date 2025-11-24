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
        // 提取标题
        metadata.title = getTagValue(reader, audio::TagType::Title);

        // 提取艺术家（使用第一个值）
        auto artists = reader.getMultiTag(audio::TagType::Artist);
        if (!artists.empty())
        {
            metadata.artist = artists[0];
        }

        // 提取专辑
        metadata.album = getTagValue(reader, audio::TagType::Album);

        // 提取专辑艺术家
        metadata.albumArtist = getTagValue(reader, audio::TagType::AlbumArtist);

        // 提取流派
        metadata.genre = getTagValue(reader, audio::TagType::Genre);

        // 提取日期
        metadata.date = getTagValue(reader, audio::TagType::Date);

        // 提取轨道号
        metadata.trackNumber = getTagValueAsInt(reader, audio::TagType::TrackNumber);

        // 提取光盘号
        metadata.discNumber = getTagValueAsInt(reader, audio::TagType::DiscNumber);

        // 提取注释
        metadata.comment = getTagValue(reader, audio::TagType::Comment);
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
        if (!value)
            return std::nullopt;

        try
        {
            // 尝试解析整数（可能包含 "1/10" 格式，只取第一部分）
            std::string str = *value;
            std::size_t slashPos = str.find('/');
            if (slashPos != std::string::npos)
            {
                str = str.substr(0, slashPos);
            }

            // 移除前导空格
            str = std::string{ core::stringUtils::stringTrim(str) };

            if (str.empty())
                return std::nullopt;

            return std::stoi(str);
        }
        catch (const std::exception&)
        {
            return std::nullopt;
        }
    }
} // namespace lms::scanner

