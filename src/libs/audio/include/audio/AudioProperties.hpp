// 音频属性结构体定义（比特率、声道等）
#pragma once

#include <cstddef>

namespace lms::audio
{
    /**
     * @brief 音频属性（简化版）
     */
    class AudioProperties
    {
    public:
        AudioProperties() = default;
        AudioProperties(std::size_t durationMs, std::size_t bitrate, std::size_t sampleRate, std::size_t channels, std::size_t bitsPerSample)
            : _durationMs{ durationMs }
            , _bitrate{ bitrate }
            , _sampleRate{ sampleRate }
            , _channels{ channels }
            , _bitsPerSample{ bitsPerSample }
        {
        }

        std::size_t getDurationMs() const { return _durationMs; }
        std::size_t getBitrate() const { return _bitrate; }
        std::size_t getSampleRate() const { return _sampleRate; }
        std::size_t getChannels() const { return _channels; }
        std::size_t getBitsPerSample() const { return _bitsPerSample; }

    private:
        std::size_t _durationMs{};
        std::size_t _bitrate{};
        std::size_t _sampleRate{};
        std::size_t _channels{};
        std::size_t _bitsPerSample{};
    };
} // namespace lms::audio

