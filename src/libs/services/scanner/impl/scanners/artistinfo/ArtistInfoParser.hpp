
#pragma once

#include <filesystem>
#include <iosfwd>
#include <span>

#include "core/Exception.hpp"

#include "types/ArtistInfo.hpp"

namespace lms::scanner
{
    class ArtistInfoParseException : public core::LmsException
    {
    public:
        using core::LmsException::LmsException;
    };

    std::span<const std::filesystem::path> getSupportedArtistInfoFiles();
    ArtistInfo parseArtistInfo(std::istream& is);
} // namespace lms::scanner
