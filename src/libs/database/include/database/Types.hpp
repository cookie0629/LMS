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
} // namespace lms::db

