
#pragma once

#include <cstddef>
#include <ostream>
#include <string>
#include <string_view>

namespace lms::core
{
    class LiteralString
    {
    public:
        constexpr LiteralString() noexcept = default;
        template<std::size_t N>
        constexpr LiteralString(const char (&str)[N]) noexcept
            : _str{ str, N - 1 }
        {
            static_assert(N > 0);
        }

        constexpr bool empty() const noexcept { return _str.empty(); }
        constexpr const char* c_str() const noexcept { return _str.data(); }
        constexpr std::size_t length() const noexcept { return _str.length(); }
        constexpr std::string_view str() const noexcept { return _str; }
        constexpr auto operator<=>(const LiteralString& other) const = default;

    private:
        std::string_view _str;
    };

    inline std::ostream& operator<<(std::ostream& os, const LiteralString& str)
    {
        os << str.str();
        return os;
    }
} // namespace lms::core

namespace std
{
    template<>
    struct hash<lms::core::LiteralString>
    {
        size_t operator()(const lms::core::LiteralString& str) const
        {
            return hash<std::string_view>{}(str.str());
        }
    };
} // namespace std

namespace lms::core
{
    struct LiteralStringHash
    {
        using hash_type = std::hash<std::string_view>;
        using is_transparent = void;

        [[nodiscard]] size_t operator()(const LiteralString& str) const
        {
            return hash_type{}(str.str());
        }

        [[nodiscard]] size_t operator()(std::string_view str) const
        {
            return hash_type{}(str);
        }

        [[nodiscard]] size_t operator()(const std::string& str) const
        {
            return hash_type{}(str);
        }
    };

    struct LiteralStringEqual
    {
        using is_transparent = void;

        [[nodiscard]] bool operator()(const LiteralString& lhs, const LiteralString& rhs) const
        {
            return lhs == rhs;
        }

        [[nodiscard]] bool operator()(const LiteralString& lhs, const std::string& rhs) const
        {
            return lhs.str() == rhs;
        }

        [[nodiscard]] bool operator()(const LiteralString& lhs, std::string_view rhs) const
        {
            return lhs.str() == rhs;
        }

        [[nodiscard]] bool operator()(const std::string_view& lhs, LiteralString rhs) const
        {
            return lhs == rhs.str();
        }

        [[nodiscard]] bool operator()(const std::string& lhs, const LiteralString& rhs) const
        {
            return lhs == rhs.str();
        }
    };
} // namespace lms::core