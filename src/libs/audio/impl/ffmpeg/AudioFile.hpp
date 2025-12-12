// 基于 FFmpeg 的音频文件读取接口声明

/*
 * 这是一个音频文件处理的头文件，使用FFmpeg库来处理音频文件。
 * 定义了音频文件相关的数据结构和类，包括容器信息、流信息、图片等。
 */
#pragma once  // 确保头文件只被包含一次，防止重复包含

#include <chrono>    // 时间相关功能，用于处理音频时长
#include <filesystem> // 文件系统操作，用于处理文件路径
#include <functional> // 函数对象，用于回调处理图片
#include <optional>   // 可选值，用于可能不存在的数据
#include <span>       // 数组视图，用于高效访问数据
#include <string>
#include <vector>     // 动态数组

#include "audio/AudioTypes.hpp" // 自定义音频类型定义

extern "C" // 使用C语言链接，与FFmpeg的C接口兼容
{
    struct AVFormatContext; // FFmpeg格式上下文结构体前向声明
}

namespace lms::audio::ffmpeg // 定义命名空间，组织音频处理相关代码
{
    struct Picture
    {
        std::string mimeType;
        std::span<const std::byte> data; // valid as long as IAudioFile exists
    };

    struct ContainerInfo
    {
        std::optional<ContainerType> container;
        std::string containerName;

        std::size_t bitrate{};
        std::chrono::milliseconds duration{};
    };

    struct StreamInfo
    {
        size_t index{};
        std::optional<CodecType> codec;
        std::string codecName;

        std::optional<size_t> bitrate;
        std::optional<std::size_t> bitsPerSample;
        std::optional<std::size_t> channelCount;
        std::optional<std::size_t> sampleRate;
    };

    class AudioFile
    {
    public:
        AudioFile(const std::filesystem::path& p);
        ~AudioFile();
        AudioFile(const AudioFile&) = delete;
        AudioFile& operator=(const AudioFile&) = delete;

        using MetadataMap = std::unordered_map<std::string, std::string>;

        const std::filesystem::path& getPath() const;
        ContainerInfo getContainerInfo() const;
        MetadataMap getMetaData() const;
        std::vector<StreamInfo> getStreamInfo() const;
        std::optional<StreamInfo> getBestStreamInfo() const;
        std::optional<std::size_t> getBestStreamIndex() const;
        bool hasAttachedPictures() const;
        void visitAttachedPictures(std::function<void(const Picture&, const MetadataMap&)> func) const;

    private:
        std::optional<StreamInfo> getStreamInfo(std::size_t streamIndex) const;

        const std::filesystem::path _p;
        AVFormatContext* _context{};
    };
} // namespace lms::audio::ffmpeg