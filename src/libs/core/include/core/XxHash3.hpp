
#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace lms::core
{
    std::uint64_t xxHash3_64(std::span<const std::byte> buf);
} // namespace lms::core