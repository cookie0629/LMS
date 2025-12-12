
#pragma once

#include <filesystem>
#include <iosfwd>
#include <span>

#include "types/PlayList.hpp"

namespace lms::scanner
{
    std::span<const std::filesystem::path> getSupportedPlayListFileExtensions();
    PlayList parsePlayList(std::istream& is);
} // namespace lms::scanner
