
#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "core/String.hpp"

namespace lms::core
{
    class UUID
    {
    public:
        static std::optional<UUID> fromString(std::string_view str);
        static UUID generate();

        std::string_view getAsString() const { return _value; }

        auto operator<=>(const UUID&) const = default;

    private:
        UUID(std::string_view value);
        std::string _value;
    };
} // namespace lms::core

namespace lms::core::stringUtils
{
    template<>
    std::optional<UUID>
    readAs(std::string_view str);
}
