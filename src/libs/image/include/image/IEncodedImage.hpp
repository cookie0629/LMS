
#pragma once

#include <cstddef>
#include <span>
#include <string_view>

namespace lms::image
{
    using ImageSize = std::size_t;

    class IEncodedImage
    {
    public:
        virtual ~IEncodedImage() = default;

        virtual std::span<const std::byte> getData() const = 0;
        virtual std::string_view getMimeType() const = 0;
    };
} // namespace lms::image