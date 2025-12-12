// TagLib 相关的工具函数声明

#pragma once

#include <filesystem>
#include <memory>
#include <span>

#include <taglib/tfile.h>

#include "audio/IAudioFileInfo.hpp"

namespace lms::audio::taglib::utils
{
    std::span<const std::filesystem::path> getSupportedExtensions();
    std::unique_ptr<::TagLib::File> parseFile(const std::filesystem::path& p, ParserOptions::AudioPropertiesReadStyle readStyle);
} // namespace lms::audio::taglib::utils