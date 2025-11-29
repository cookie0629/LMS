#pragma once

#include <cstdint>
#include <functional>
#include <vector>

namespace lms::db
{
    /**
     * @brief 范围结构，用于分页查询
     */
    struct Range
    {
        std::size_t offset{};
        std::size_t size{};

        bool operator==(const Range& rhs) const 
        { 
            return offset == rhs.offset && size == rhs.size; 
        }
    };

    /**
     * @brief 范围查询结果
     */
    template<typename T>
    struct RangeResults
    {
        Range range;
        std::vector<T> results;
        bool moreResults{};
    };

    /**
     * @brief 用户类型枚举
     */
    enum class UserType
    {
        REGULAR,  // 普通用户
        ADMIN,    // 管理员
        DEMO,     // 演示用户
    };

    enum class ImageType
    {
        Unknown,
        Front,
        Back,
        Artist,
    };

    /**
     * @brief Track 和 Artist 之间的关联类型
     */
    enum class TrackArtistLinkType
    {
        Artist = 0,      // 常规轨道艺术家
        Arranger = 1,    // 编曲者
        Composer = 2,    // 作曲者
        Conductor = 3,   // 指挥
        Lyricist = 4,    // 作词者
        Mixer = 5,       // 混音师
        Performer = 6,   // 表演者
        Producer = 7,    // 制作人
        ReleaseArtist = 8, // 专辑艺术家
        Remixer = 9,     // 混音者
        Writer = 10,     // 作者
    };
} // namespace lms::db

