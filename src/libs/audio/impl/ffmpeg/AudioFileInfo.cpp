// 使用 FFmpeg 提取音频元数据

/*
 * AudioFileInfo 类的实现文件
 * 该类用于获取音频文件的属性信息、标签信息和图片信息
 */
#include "AudioFileInfo.hpp"

#include "audio/AudioTypes.hpp"
#include "audio/IAudioFileInfo.hpp"

#include "AudioFile.hpp"
#include "ImageReader.hpp"
#include "TagReader.hpp"

namespace lms::audio::ffmpeg
{
    // 命名空间内的匿名命名空间，用于定义辅助函数
    namespace
    {
        // 计算音频属性的辅助函数
        // 参数: audioFile - 音频文件对象
        // 返回: 计算得到的音频属性对象
        // 异常: 如果无法获取必要的音频信息，抛出AudioFileParsingException
        AudioProperties computeAudioProperties(const AudioFile& audioFile)
        {
            // 初始化音频属性对象
            AudioProperties audioProperties;

            // 获取容器信息和最佳流信息
            const auto containerInfo{ audioFile.getContainerInfo() };
            const auto bestStreamInfo{ audioFile.getBestStreamInfo() };
            // 检查是否能找到最佳音频流
            if (!bestStreamInfo)
                throw AudioFileParsingException{ "Cannot find best audio stream" };

            // 检查容器类型是否受支持
            if (!containerInfo.container)
                throw AudioFileParsingException{ "Unhandled container type '" + containerInfo.containerName + "'" };

            // 检查编解码器类型是否受支持
            if (!bestStreamInfo->codec)
                throw AudioFileParsingException{ "Unhandled codec type '" + bestStreamInfo->codecName + "'" };

            // 检查是否能获取比特率
            if (!bestStreamInfo->bitrate || *bestStreamInfo->bitrate == 0)
                throw AudioFileParsingException{ "Cannot determine bitrate" };

            // 检查是否能获取声道数
            if (!bestStreamInfo->channelCount || *bestStreamInfo->channelCount == 0)
                throw AudioFileParsingException{ "Cannot determine channel count" };

            // 检查是否能获取采样率
            if (!bestStreamInfo->sampleRate || *bestStreamInfo->sampleRate == 0)
                throw AudioFileParsingException{ "Cannot determine sample rate" };

            // 设置音频属性
            audioProperties.container = *containerInfo.container;
            audioProperties.duration = containerInfo.duration;
            audioProperties.codec = *bestStreamInfo->codec;
            audioProperties.bitrate = *bestStreamInfo->bitrate;
            audioProperties.channelCount = *bestStreamInfo->channelCount;
            audioProperties.sampleRate = *bestStreamInfo->sampleRate;
            audioProperties.bitsPerSample = bestStreamInfo->bitsPerSample;

            return audioProperties;
        }
    } // namespace

    // 构造函数
    // 参数: filePath - 音频文件路径
    //       enableExtraDebugLogs - 是否启用额外的调试日志
    AudioFileInfo::AudioFileInfo(const std::filesystem::path& filePath, bool enableExtraDebugLogs)
        : _audioFile{ std::make_unique<AudioFile>(filePath) }
        , _audioProperties{ std::make_unique<AudioProperties>(computeAudioProperties(*_audioFile)) }
        , _tagReader{ std::make_unique<TagReader>(*_audioFile, enableExtraDebugLogs) }
        , _imageReader{ std::make_unique<ImageReader>(*_audioFile) }
    {
    }

    // 析构函数
    AudioFileInfo::~AudioFileInfo() = default;

    // 获取音频属性
    // 返回: 音频属性的常量引用
    const AudioProperties& AudioFileInfo::getAudioProperties() const
    {
        return *_audioProperties;
    }

    // 获取图片读取器
    // 返回: 图片读取器的常量引用
    const IImageReader& AudioFileInfo::getImageReader() const
    {
        return *_imageReader;
    }

    // 获取标签读取器
    // 返回: 标签读取器的常量引用
    const ITagReader& AudioFileInfo::getTagReader() const
    {
        return *_tagReader;
    }

} // namespace lms::audio::ffmpeg
