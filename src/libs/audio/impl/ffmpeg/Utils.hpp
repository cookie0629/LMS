// FFmpeg 音频处理的通用工具函数声明

// 防止头文件被重复包含
#pragma once

// 包含文件系统相关的头文件，用于处理文件路径
#include <filesystem>
// 包含 span 相关的头文件，用于提供对连续内存序列的视图
#include <span>

// 定义命名空间 lms::audio::ffmpeg::utils，用于封装 FFmpeg 相关的工具函数
namespace lms::audio::ffmpeg::utils
{
    // 获取 FFmpeg 支持的文件扩展名的函数声明
    // 返回类型为 std::span<const std::filesystem::path>，表示一个只读的文件路径序列视图
    std::span<const std::filesystem::path> getSupportedExtensions();
} // namespace lms::audio::ffmpeg::utils