
#pragma once

#include <cstddef>

namespace lms::image
{
    using ImageSize = std::size_t;

    struct ImageProperties
    {
        ImageSize width{};
        ImageSize height{};
    };
} // namespace lms::image