
#pragma once

#include <optional>
#include <string_view>

#include "core/String.hpp"

namespace lms::api::subsonic
{
    struct ProtocolVersion
    {
        unsigned major{};
        unsigned minor{};
        unsigned patch{};
    };

    static inline constexpr ProtocolVersion defaultServerProtocolVersion{ 1, 16, 1 };
} // namespace lms::api::subsonic

namespace lms::core::stringUtils
{
    template<>
    std::optional<api::subsonic::ProtocolVersion> readAs(std::string_view str);
}
