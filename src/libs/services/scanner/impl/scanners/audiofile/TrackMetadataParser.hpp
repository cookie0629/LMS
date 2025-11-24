#pragma once

#include <optional>
#include <string>
#include <vector>

#include "audio/ITagReader.hpp"

namespace lms::scanner
{
    /**
     * @brief 轨道元数据结构（简化版）
     */
    struct TrackMetadata
    {
        std::optional<std::string> title;
        std::optional<std::string> artist;
        std::optional<std::string> album;
        std::optional<std::string> albumArtist;
        std::optional<std::string> genre;
        std::optional<std::string> date;
        std::optional<int> trackNumber;
        std::optional<int> discNumber;
        std::optional<std::string> comment;
    };

    /**
     * @brief 轨道元数据解析器（简化版）
     * 
     * 从音频标签中提取元数据信息
     */
    class TrackMetadataParser
    {
    public:
        /**
         * @brief 解析参数
         */
        struct Parameters
        {
            std::vector<std::string> artistTagDelimiters; // 艺术家标签分隔符
            std::vector<std::string> defaultTagDelimiters;  // 默认标签分隔符

            Parameters()
                : artistTagDelimiters{ ";", ",", "/" }
                , defaultTagDelimiters{ ";", ",", "/" }
            {
            }
        };

        TrackMetadataParser(const Parameters& params = Parameters{})
            : _params{ params }
        {
        }

        ~TrackMetadataParser() = default;
        TrackMetadataParser(const TrackMetadataParser&) = delete;
        TrackMetadataParser& operator=(const TrackMetadataParser&) = delete;

        /**
         * @brief 从标签读取器解析轨道元数据
         */
        TrackMetadata parseTrackMetadata(const audio::ITagReader& reader) const;

    private:
        /**
         * @brief 处理标签并填充元数据
         */
        void processTags(const audio::ITagReader& reader, TrackMetadata& metadata) const;

        /**
         * @brief 获取标签值（第一个匹配）
         */
        std::optional<std::string> getTagValue(const audio::ITagReader& reader, audio::TagType tagType) const;

        /**
         * @brief 获取整数标签值
         */
        std::optional<int> getTagValueAsInt(const audio::ITagReader& reader, audio::TagType tagType) const;

        const Parameters _params;
    };
} // namespace lms::scanner

