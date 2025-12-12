// 强类型包装/标签类型工具

#pragma once

namespace lms::core
{
    template<typename Tag, typename T>
    class TaggedType
    {
    public:
        using underlying_type = T;

        explicit constexpr TaggedType() = default;
        explicit constexpr TaggedType(T value)
            : _value{ value } {}

        constexpr T value() const { return _value; }

        auto operator<=>(const TaggedType&) const = default;

    private:
        T _value{};
    };

    template<typename Tag>
    using TaggedBool = TaggedType<Tag, bool>;
} // namespace lms::core