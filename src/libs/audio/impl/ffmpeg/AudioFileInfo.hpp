// 使用 FFmpeg 提取音频元数据的接口声明

// 防止头文件被重复包含
#pragma once

// 文件系统库，用于处理文件路径
#include <filesystem>

// 音频相关类型定义的头文件
#include "audio/AudioTypes.hpp"
// 音频文件信息接口定义的头文件
#include "audio/IAudioFileInfo.hpp"

// 定义命名空间 lms::audio::ffmpeg
namespace lms::audio::ffmpeg
{
    // 前向声明 AudioFile 类，用于处理音频文件
    class AudioFile;
    // 前向声明 TagReader 类，用于读取音频标签信息
    class TagReader;
    // 前向声明 ImageReader 类，用于读取音频文件关联的图片信息
    class ImageReader;

    // AudioFileInfo 类，实现 IAudioFileInfo 接口，用于获取音频文件的各种信息
    class AudioFileInfo final : public IAudioFileInfo
    {
    public:
        // 构造函数，接收文件路径和是否启用额外调试日志的标志
        AudioFileInfo(const std::filesystem::path& filePath, bool enableExtraDebugLogs);
        // 析构函数
        ~AudioFileInfo();
        // 禁用拷贝构造函数
        AudioFileInfo(const AudioFileInfo&) = delete;
        // 禁用拷贝赋值运算符
        AudioFileInfo& operator=(const AudioFileInfo&) = delete;

    private:
        // 获取音频属性的方法，重写自接口
        const AudioProperties& getAudioProperties() const override;
        // 获取图片读取器的方法，重写自接口
        const IImageReader& getImageReader() const override;
        // 获取标签读取器的方法，重写自接口
        const ITagReader& getTagReader() const override;

        // 使用智能指针管理音频文件对象
        std::unique_ptr<AudioFile> _audioFile;
        // 使用智能指针管理音频属性对象
        std::unique_ptr<AudioProperties> _audioProperties;
        // 使用智能指针管理标签读取器对象
        std::unique_ptr<TagReader> _tagReader;
        // 使用智能指针管理图片读取器对象
        std::unique_ptr<ImageReader> _imageReader;
    };
} // namespace lms::audio::ffmpeg
