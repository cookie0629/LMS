
#include "core/MimeTypes.hpp"

#include <unordered_map>

#include "core/String.hpp"

namespace lms::core
{
    std::string_view getMimeType(const std::filesystem::path& fileExtension)
    {
        static const std::unordered_map<std::string, std::string_view> entries{
            // audio
            { ".aac", "audio/aac" },
            { ".ac3", "audio/ac3" },
            { ".aif", "audio/x-aiff" },
            { ".aiff", "audio/x-aiff" },
            { ".alac", "audio/mp4" },
            { ".ape", "audio/x-monkeys-audio" },
            { ".dff", "audio/x-dsd-dff" },
            { ".dsdiff", "audio/x-dsd-diff" },
            { ".dsf", "audio/x-dsd" },
            { ".dsf", "audio/x-dsd-dsf" },
            { ".dts", "audio/vnd.dts" },
            { ".dtshd", "audio/vnd.dts.hd" },
            { ".eac3", "audio/eac3" },
            { ".flac", "audio/flac" },
            { ".m3u", "audio/x-mpegurl" },
            { ".m4a", "audio/mp4" },
            { ".m4b", "audio/mp4" },
            { ".mka", "audio/x-matroska" },
            { ".mka", "audio/x-matroska" },
            { ".mp3", "audio/mpeg" },
            { ".mpc", "audio/x-musepack" },
            { ".oga", "audio/ogg" },
            { ".ogg", "audio/ogg" },
            { ".opus", "audio/opus" },
            { ".pls", "audio/x-scpls" },
            { ".shn", "audio/x-shn" },
            { ".tta", "audio/x-tta" },
            { ".wav", "audio/x-wav" },
            { ".webm", "audio/webm" },
            { ".wma", "audio/x-ms-wma" },
            { ".wv", "audio/x-wavpack" },
            { ".wvp", "audio/x-wavpack" },

            // image
            { ".bmp", "image/bmp" },
            { ".gif", "image/gif" },
            { ".jpg", "image/jpeg" },
            { ".jpeg", "image/jpeg" },
            { ".png", "image/png" },
            { ".svg", "image/svg+xml" },
            { ".webp", "image/webp" },
        };

        auto it{ entries.find(core::stringUtils::stringToLower(fileExtension.c_str())) };
        if (it == std::cend(entries))
            return "application/octet-stream";

        return it->second;
    }
} // namespace lms::core