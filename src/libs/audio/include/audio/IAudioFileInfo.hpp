#pragma once

#include <filesystem>
#include <memory>
#include <span>

#include "audio/AudioProperties.hpp"
#include "audio/Exception.hpp"
#include "audio/IImageReader.hpp"
#include "audio/ITagReader.hpp"

namespace lms::audio
{
    /**
     * @brief 音频文件信息接口
     */
    class IAudioFileInfo
    {
    public:
        virtual ~IAudioFileInfo() = default;

        /**
         * @brief 获取音频属性
         */
        virtual const AudioProperties& getAudioProperties() const = 0;

        /**
         * @brief 获取图像读取器
         */
        virtual const IImageReader& getImageReader() const = 0;

        /**
         * @brief 获取标签读取器
         */
        virtual const ITagReader& getTagReader() const = 0;
    };

    /**
     * @brief 音频文件解析异常
     */
    class AudioFileParsingException : public Exception
    {
    public:
        using Exception::Exception;
    };

    /**
     * @brief IO 异常
     */
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

    /**
     * @brief 解析器选项
     */
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

    /**
     * @brief 解析音频文件（简化版：返回空实现）
     * 
     * 注意：完整版本需要集成音频处理库（如 TagLib）
     */
    std::unique_ptr<IAudioFileInfo> parseAudioFile(const std::filesystem::path& p, const ParserOptions& parserOptions = ParserOptions{});

    /**
     * @brief 获取支持的扩展名
     */
    std::span<const std::filesystem::path> getSupportedExtensions(ParserOptions::Parser parser);
} // namespace lms::audio

