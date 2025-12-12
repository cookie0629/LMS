// 部分日期时间类型声明
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace Wt
{
    class WDateTime;
}

namespace lms::core
{
    class PartialDateTime
    {
    public:
        constexpr PartialDateTime() = default;
        PartialDateTime(int year);
        PartialDateTime(int year, unsigned month);
        PartialDateTime(int year, unsigned month, unsigned day);
        PartialDateTime(int year, unsigned month, unsigned day, unsigned hour, unsigned min, unsigned sec);

        static PartialDateTime fromString(std::string_view str);
        static PartialDateTime fromWtDateTime(const Wt::WDateTime& dateTime);
        std::string toString() const;

        constexpr bool isValid() const { return _precision != Precision::Invalid; }

        enum class Precision : std::uint8_t
        {
            Invalid,
            Year,
            Month,
            Day,
            Hour,
            Min,
            Sec,
        };
        constexpr Precision getPrecision() const { return _precision; }

        constexpr std::optional<int> getYear() const { return (_precision >= Precision::Year ? std::make_optional(_year) : std::nullopt); }
        constexpr std::optional<int> getMonth() const { return (_precision >= Precision::Month ? std::make_optional(_month) : std::nullopt); }
        constexpr std::optional<int> getDay() const { return (_precision >= Precision::Day ? std::make_optional(_day) : std::nullopt); }
        constexpr std::optional<int> getHour() const { return (_precision >= Precision::Hour ? std::make_optional(_hour) : std::nullopt); }
        constexpr std::optional<int> getMin() const { return (_precision >= Precision::Min ? std::make_optional(_min) : std::nullopt); }
        constexpr std::optional<int> getSec() const { return (_precision >= Precision::Sec ? std::make_optional(_sec) : std::nullopt); }

        constexpr auto operator<=>(const PartialDateTime& other) const = default;

    private:
        static PartialDateTime parseDateTime(const char* format, const std::string& dateTimeStr);

        std::int16_t _year{};
        std::uint8_t _month{}; // 1 to 12
        std::uint8_t _day{};   // 1 to 31
        std::uint8_t _hour{};  // 0 to 23
        std::uint8_t _min{};   // 0 to 59
        std::uint8_t _sec{};   // 0 to 59
        Precision _precision{ Precision::Invalid };
    };
} // namespace lms::core