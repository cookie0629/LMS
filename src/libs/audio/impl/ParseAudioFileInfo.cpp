// 将底层解析结果转换为统一音频文件信息结构

/*
 * @file: 音频文件解析相关实现
 * @description: 包含音频文件解析功能的实现，支持TagLib和FFmpeg两种解析方式
 */
#include <memory>  // 智能指针相关头文件

#include "audio/IAudioFileInfo.hpp"  // 音频文件信息接口头文件

#include "ffmpeg/AudioFileInfo.hpp"  // FFmpeg实现的音频文件信息类头文件
#include "ffmpeg/Utils.hpp"         // FFmpeg工具函数头文件
#include "taglib/AudioFileInfo.hpp"  // TagLib实现的音频文件信息类头文件
#include "taglib/Utils.hpp"         // TagLib工具函数头文件

namespace lms::audio
{
    std::unique_ptr<IAudioFileInfo> parseAudioFile(const std::filesystem::path& p, const ParserOptions& parserOptions)
    {
        switch (parserOptions.parser)
        {
        case ParserOptions::Parser::TagLib:
            return std::make_unique<taglib::AudioFileInfo>(p, parserOptions.readStyle, parserOptions.enableExtraDebugLogs);
        case ParserOptions::Parser::FFmpeg:
            return std::make_unique<ffmpeg::AudioFileInfo>(p, parserOptions.enableExtraDebugLogs);
        }

        return {};
    }

    std::span<const std::filesystem::path> getSupportedExtensions(ParserOptions::Parser parser)
    {
        switch (parser)
        {
        case ParserOptions::Parser::TagLib:
            return taglib::utils::getSupportedExtensions();
        case ParserOptions::Parser::FFmpeg:
            return ffmpeg::utils::getSupportedExtensions();
        }

        return {};
    }

} // namespace lms::audio