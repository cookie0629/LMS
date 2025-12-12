
#pragma once

#include <cstddef>
#include <filesystem>
#include <span>

#include <Magick++.h>

#include "image/IRawImage.hpp"

namespace lms::image::GraphicsMagick
{
    class RawImage : public IRawImage
    {
    public:
        RawImage(std::span<const std::byte> encodedData);
        RawImage(const std::filesystem::path& path);

        ImageSize getWidth() const override;
        ImageSize getHeight() const override;

        void resize(ImageSize width) override;

        Magick::Image getMagickImage() const;

    private:
        Magick::Image _image;
    };
} // namespace lms::image::GraphicsMagick
