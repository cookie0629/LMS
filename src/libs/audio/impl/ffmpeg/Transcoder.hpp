// 基于 FFmpeg 的音频转码器接口

#pragma once

#include "audio/ITranscoder.hpp"

namespace lms::core
{
    class IChildProcess;
}

namespace lms::audio::ffmpeg
{
    class Transcoder : public ITranscoder
    {
    public:
        Transcoder(const TranscodeParameters& parameters);
        ~Transcoder() override;
        Transcoder(const Transcoder&) = delete;
        Transcoder& operator=(const Transcoder&) = delete;

    private:
        void asyncRead(std::byte* buffer, std::size_t bufferSize, ReadCallback) override;
        std::size_t readSome(std::byte* buffer, std::size_t bufferSize) override;

        std::string_view getOutputMimeType() const override;
        const TranscodeOutputParameters& getOutputParameters() const override { return _outputParams; }

        bool finished() const override;
        static void init();
        void start();

        const std::size_t _debugId{};
        const TranscodeInputParameters _inputParams;
        const TranscodeOutputParameters _outputParams;
        std::unique_ptr<core::IChildProcess> _childProcess;
    };
} // namespace lms::audio::ffmpeg