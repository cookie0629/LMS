#pragma once

#include <filesystem>
#include <iosfwd>
#include <span>

#include "types/Lyrics.hpp"

namespace lms::scanner
{
    std::span<const std::filesystem::path> getSupportedLyricsFileExtensions();
    Lyrics parseLyrics(std::istream& is);
} // namespace lms::scanner

