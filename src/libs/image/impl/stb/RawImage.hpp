
#pragma once

#include <cstddef>
#include <filesystem>
#include <span>

#include "image/IRawImage.hpp"

namespace lms::image::STB
{
    class RawImage : public IRawImage
    {
    public:
        RawImage(std::span<const std::byte> encodedData);
        RawImage(const std::filesystem::path& path);

        ~RawImage() override = default;
        RawImage(const RawImage&) = delete;
        RawImage& operator=(const RawImage&) = delete;

        ImageSize getWidth() const override;
        ImageSize getHeight() const override;

        void resize(ImageSize width) override;

        const std::byte* getData() const;

    private:
        int _width{};
        int _height{};
        using UniquePtrFree = std::unique_ptr<unsigned char, decltype(&std::free)>;
        UniquePtrFree _data{ nullptr, std::free };
    };
} // namespace lms::image::STB
