#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace lms::audio
{
    /**
     * @brief 标签类型（简化版）
     */
    enum class TagType
    {
        Title,
        Artist,
        Album,
        AlbumArtist,
        Genre,
        Date,
        TrackNumber,
        DiscNumber,
        Comment,
    };

    /**
     * @brief 标签读取器接口（简化版）
     */
    class ITagReader
    {
    public:
        virtual ~ITagReader() = default;

        /**
         * @brief 获取标签值
         * @param tagType 标签类型
         * @return 标签值（如果存在）
         */
        virtual std::optional<std::string> getTag(TagType tagType) const = 0;

        /**
         * @brief 获取多个标签值（用于支持多个艺术家等）
         * @param tagType 标签类型
         * @return 标签值列表
         */
        virtual std::vector<std::string> getMultiTag(TagType tagType) const = 0;

        /**
         * @brief 检查标签是否存在
         */
        virtual bool hasTag(TagType tagType) const = 0;
    };

    /**
     * @brief 标签读取器简化实现（空实现）
     */
    class TagReader : public ITagReader
    {
    public:
        std::optional<std::string> getTag(TagType tagType) const override
        {
            return std::nullopt;
        }

        std::vector<std::string> getMultiTag(TagType tagType) const override
        {
            return {};
        }

        bool hasTag(TagType tagType) const override
        {
            return false;
        }
    };
} // namespace lms::audio

