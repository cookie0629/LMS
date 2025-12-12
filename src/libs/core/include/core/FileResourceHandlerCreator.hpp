// 文件资源处理器工厂声明

#pragma once

#include <filesystem>
#include <memory>
#include <string_view>

#include "core/IResourceHandler.hpp"

namespace lms::core
{
    // createFileResourceHandler: 创建用于静态文件（CSS/JS/图片等）的资源处理器。
    // createFileResourceHandler: создаёт обработчик ресурсов для статических файлов（CSS/JS/картинки и т.п.）。
    std::unique_ptr<IResourceHandler> createFileResourceHandler(const std::filesystem::path& path, std::string_view mimeType = "");
}