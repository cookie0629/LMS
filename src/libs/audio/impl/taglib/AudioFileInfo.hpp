
/*
 * AudioFileInfo 类的实现文件，提供音频文件信息的读取功能
 * 该类继承自 IAudioFileInfo 接口，实现了音频文件属性、标签和图片的读取
 */
#pragma once 

#include "audio/AudioTypes.hpp"
#include "audio/IAudioFileInfo.hpp"
#include "audio/IImageReader.hpp"
#include "audio/ITagReader.hpp"

namespace TagLib
{
    class File;
}

namespace lms::audio::taglib
{
    class ImageReader;
    class TagReader;

    class AudioFileInfo final : public IAudioFileInfo
    {
    public:
        AudioFileInfo(const std::filesystem::path& filePath, ParserOptions::AudioPropertiesReadStyle readStyle, bool enableExtraDebugLogs);
        ~AudioFileInfo() override;

        AudioFileInfo(const AudioFileInfo&) = delete;
        AudioFileInfo& operator=(const AudioFileInfo&) = delete;

    private:
        const AudioProperties& getAudioProperties() const override;
        const IImageReader& getImageReader() const override;
        const ITagReader& getTagReader() const override;

        const std::filesystem::path _filePath;
        std::unique_ptr<::TagLib::File> _file;
        std::unique_ptr<AudioProperties> _audioProperties;
        std::unique_ptr<TagReader> _tagReader;
        std::unique_ptr<ImageReader> _imageReader;
    };
} // namespace lms::audio::taglib
