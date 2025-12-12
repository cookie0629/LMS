
#pragma once

#include <chrono>
#include <filesystem>
#include <optional>

namespace lms::audio
{
    // TODO deprecate?
    enum class OutputFormat
    {
        MP3,
        OGG_OPUS,
        MATROSKA_OPUS,
        OGG_VORBIS,
        WEBM_VORBIS,
    };

    struct TranscodeInputParameters
    {
        std::filesystem::path filePath;
        std::chrono::milliseconds duration{}; // Duration of the audio file
        std::chrono::milliseconds offset{};   // Offset in the audio file to start transcoding from
    };

    struct TranscodeOutputParameters
    {
        std::optional<OutputFormat> format;
        std::optional<unsigned> bitrate;
        std::optional<unsigned> bitsPerSample;
        std::optional<unsigned> channelCount;
        std::optional<unsigned> sampleRate;
        bool stripMetadata{ true };
    };

    struct TranscodeParameters
    {
        TranscodeInputParameters inputParameters;
        TranscodeOutputParameters outputParameters;
    };
} // namespace lms::audio