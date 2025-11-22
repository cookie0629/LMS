#pragma once

#include <cstdint>
#include <type_traits>

namespace lms::core
{
    /**
     * @brief 标记类型模板，用于创建强类型别名
     * 
     * 使用示例：
     * using UserId = TaggedType<int64_t, struct UserIdTag>;
     */
    template<typename T, typename Tag>
    class TaggedType
    {
    public:
        using ValueType = T;

        TaggedType() = default;
        explicit TaggedType(const T& value) : _value(value) {}
        explicit TaggedType(T&& value) : _value(std::move(value)) {}

        T& getValue() { return _value; }
        const T& getValue() const { return _value; }

        operator T&() { return _value; }
        operator const T&() const { return _value; }

        bool operator==(const TaggedType& other) const { return _value == other._value; }
        bool operator!=(const TaggedType& other) const { return _value != other._value; }
        bool operator<(const TaggedType& other) const { return _value < other._value; }
        bool operator>(const TaggedType& other) const { return _value > other._value; }
        bool operator<=(const TaggedType& other) const { return _value <= other._value; }
        bool operator>=(const TaggedType& other) const { return _value >= other._value; }

    private:
        T _value{};
    };
} // namespace lms::core

