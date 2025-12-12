
#pragma once

#include "image/Types.hpp"

namespace lms::image
{
    class IRawImage
    {
    public:
        virtual ~IRawImage() = default;

        virtual ImageSize getWidth() const = 0;
        virtual ImageSize getHeight() const = 0;

        virtual void resize(ImageSize width) = 0;
    };
} // namespace lms::image
