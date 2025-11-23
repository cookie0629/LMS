#include "audio/IAudioFileInfo.hpp"

#include "audio/AudioProperties.hpp"
#include "audio/Exception.hpp"
#include "audio/IImageReader.hpp"
#include "audio/ITagReader.hpp"

namespace lms::audio
{
    /**
     * @brief 音频文件信息简化实现（空实现）
     */
    class AudioFileInfo : public IAudioFileInfo
    {
    public:
        AudioFileInfo()
            : _audioProperties{}
            , _imageReader{}
            , _tagReader{}
        {
        }

        const AudioProperties& getAudioProperties() const override
        {
            return _audioProperties;
        }

        const IImageReader& getImageReader() const override
        {
            return _imageReader;
        }

        const ITagReader& getTagReader() const override
        {
            return _tagReader;
        }

    private:
        AudioProperties _audioProperties;
        ImageReader _imageReader;
        TagReader _tagReader;
    };

    std::unique_ptr<IAudioFileInfo> parseAudioFile(const std::filesystem::path& p, const ParserOptions& parserOptions)
    {
        // 简化版：返回空实现
        // 完整版本需要：
        // 1. 根据文件扩展名选择解析器（TagLib 或 FFmpeg）
        // 2. 使用音频处理库读取文件
        // 3. 提取音频属性和标签信息
        // 4. 返回包含实际数据的 IAudioFileInfo 实现

        (void)p;           // 未使用
        (void)parserOptions; // 未使用

        return std::make_unique<AudioFileInfo>();
    }

    std::span<const std::filesystem::path> getSupportedExtensions(ParserOptions::Parser parser)
    {
        // 简化版：返回空列表
        // 完整版本需要根据解析器返回支持的扩展名列表

        (void)parser; // 未使用

        static const std::vector<std::filesystem::path> empty;
        return empty;
    }
} // namespace lms::audio

