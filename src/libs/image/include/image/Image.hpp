
#pragma once

#include <filesystem>
#include <memory>
#include <span>

#include "image/IEncodedImage.hpp"
#include "image/IRawImage.hpp"

namespace lms::image
{
    void init(const std::filesystem::path& path);
    std::span<const std::filesystem::path> getSupportedFileExtensions();

    // All these methods may throw Exception
    ImageProperties probeImage(const std::filesystem::path& path);
    ImageProperties probeImage(std::span<const std::byte> encodedData);

    std::unique_ptr<IRawImage> decodeImage(std::span<const std::byte> encodedData);
    std::unique_ptr<IRawImage> decodeImage(const std::filesystem::path& path);

    std::unique_ptr<IEncodedImage> readImage(std::span<const std::byte> encodedData, std::string_view mimeType);
    std::unique_ptr<IEncodedImage> readImage(const std::filesystem::path& path, std::string_view mimeType = ""); // mimeType may already been known, otherwise, it is guessed based on the file extension

    std::unique_ptr<IEncodedImage> encodeToJPEG(const IRawImage& rawImage, unsigned quality);
} // namespace lms::image