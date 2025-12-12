
#include "Utils.hpp"

namespace lms::audio::ffmpeg::utils
{
    std::span<const std::filesystem::path> getSupportedExtensions()
    {
        // TODO: list demuxers to retrieve supported formats
        static const std::array<std::filesystem::path, 18> fileExtensions{
            ".aac",
            ".alac",
            ".aif",
            ".aiff",
            ".ape",
            ".dsf",
            ".flac",
            ".m4a",
            ".m4b",
            ".mp3",
            ".mpc",
            ".oga",
            ".ogg",
            ".opus",
            ".shn",
            ".wav",
            ".wma",
            ".wv",
        };
        return fileExtensions;
    }
} // namespace lms::audio::ffmpeg::utils