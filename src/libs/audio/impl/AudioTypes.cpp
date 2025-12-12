
/**
 * @file AudioTypes.hpp
 * @brief 音频类型相关的转换函数实现
 */
#include "audio/AudioTypes.hpp"

namespace lms::audio
{
    /**
     * @brief 将容器类型(ContainerType)转换为对应的字符串表示
     * @param type 容器类型枚举值
     * @return 返回容器类型对应的字符串，如果类型未知则返回空字符串
     */
    core::LiteralString containerTypeToString(ContainerType type)
    {
        switch (type)
        {
        case ContainerType::AIFF:
            return "AIFF";
        case ContainerType::APE:
            return "APE";
        case ContainerType::ASF:
            return "ASF";
        case ContainerType::DSF:
            return "DSF";
        case ContainerType::FLAC:
            return "FLAC";
        case ContainerType::MP4:
            return "MP4";
        case ContainerType::MPC:
            return "MPC";
        case ContainerType::MPEG:
            return "MPEG";
        case ContainerType::Ogg:
            return "Ogg";
        case ContainerType::Shorten:
            return "Shorten";
        case ContainerType::TrueAudio:
            return "TrueAudio";
        case ContainerType::WAV:
            return "WAV";
        case ContainerType::WavPack:
            return "WavPack";
        }

        return "";
    }

    core::LiteralString codecTypeToString(CodecType type)
    {
        switch (type)
        {
        case CodecType::AAC:
            return "AAC";
        case CodecType::ALAC:
            return "ALAC";
        case CodecType::APE:
            return "APE";
        case CodecType::DSD:
            return "DSD";
        case CodecType::FLAC:
            return "FLAC";
        case CodecType::MP3:
            return "MP3";
        case CodecType::MP4ALS:
            return "MP4ALS";
        case CodecType::MPC7:
            return "MPC7";
        case CodecType::MPC8:
            return "MPC8";
        case CodecType::Opus:
            return "Opus";
        case CodecType::PCM:
            return "PCM";
        case CodecType::Shorten:
            return "Shorten";
        case CodecType::TrueAudio:
            return "TrueAudio";
        case CodecType::Vorbis:
            return "Vorbis";
        case CodecType::WavPack:
            return "WavPack";
        case CodecType::WMA1:
            return "WMA1";
        case CodecType::WMA2:
            return "WMA2";
        case CodecType::WMA9Pro:
            return "WMA9Pro";
        case CodecType::WMA9Lossless:
            return "WMA9Lossless";
        }

        return "";
    }
} // namespace lms::audio