
#pragma once

#include <chrono>
#include <optional>

#include "core/LiteralString.hpp"

namespace lms::audio
{
    enum class ContainerType
    {
        AIFF,
        APE, // Monkey's Audio
        ASF, // Advanced Systems Format
        DSF,
        FLAC,
        MP4,
        MPC, // Musepack
        MPEG,
        Ogg,
        Shorten,
        TrueAudio,
        WAV,
        WavPack,
    };

    core::LiteralString containerTypeToString(ContainerType type);

    enum class CodecType
    {
        AAC,
        ALAC, // Apple Lossless Audio Codec (ALAC)
        APE,  // Monkey's Audio
        DSD,  // DSD
        FLAC, // Flac
        MP3,
        MP4ALS, // MPEG-4 Audio Lossless Coding
        MPC7,   // Musepack
        MPC8,   // Musepack
        Opus,   // Opus
        PCM,
        Shorten, // Shorten (shn)
        TrueAudio,
        Vorbis,
        WavPack, // WavPack
        WMA1,
        WMA2,
        WMA9Pro,
        WMA9Lossless,
    };

    core::LiteralString codecTypeToString(CodecType type);

    struct AudioProperties
    {
        ContainerType container;
        CodecType codec;
        std::chrono::milliseconds duration;
        unsigned bitrate;
        unsigned channelCount;
        unsigned sampleRate;
        std::optional<unsigned> bitsPerSample;
    };
} // namespace lms::audio