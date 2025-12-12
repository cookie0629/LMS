// XxHash3 哈希计算实现

#include "core/XxHash3.hpp"

#define XXH_INLINE_ALL
#include <xxhash.h>

namespace lms::core
{
    std::uint64_t xxHash3_64(std::span<const std::byte> buf)
    {
        return XXH3_64bits(buf.data(), buf.size());
    }
} // namespace lms::core