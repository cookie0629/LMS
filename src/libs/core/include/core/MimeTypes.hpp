
#pragma once

#include <filesystem>
#include <string_view>

namespace lms::core
{
    std::string_view getMimeType(const std::filesystem::path& fileExtension);
}