
#pragma once

#include <filesystem>
#include <span>

#include "audio/Exception.hpp"

namespace lms::audio
{
    class AudioProperties;
    class IImageReader;
    class ITagReader;

    class IAudioFileInfo
    {
    public:
        virtual ~IAudioFileInfo() = default;

        virtual const AudioProperties& getAudioProperties() const = 0;
        virtual const IImageReader& getImageReader() const = 0;
        virtual const ITagReader& getTagReader() const = 0;
    };

    class AudioFileParsingException : public Exception
    {
    public:
        using Exception::Exception;
    };

    class IOException : public Exception
    {
    public:
        IOException(std::string_view message, std::error_code err)
            : Exception{ std::string{ message } + ": " + err.message() }
            , _err{ err }
        {
        }

        std::error_code getErrorCode() const { return _err; }

    private:
        std::error_code _err;
    };

    struct ParserOptions
    {
        enum class Parser
        {
            TagLib,
            FFmpeg,
        };

        enum class AudioPropertiesReadStyle
        {
            Fast,
            Average,
            Accurate,
        };

        Parser parser{ Parser::TagLib };
        AudioPropertiesReadStyle readStyle{ AudioPropertiesReadStyle::Average };
        bool enableExtraDebugLogs{};
    };
    std::unique_ptr<IAudioFileInfo> parseAudioFile(const std::filesystem::path& p, const ParserOptions& parserOptions = ParserOptions{});

    std::span<const std::filesystem::path> getSupportedExtensions(ParserOptions::ParserOptions::Parser parser);
} // namespace lms::audio
