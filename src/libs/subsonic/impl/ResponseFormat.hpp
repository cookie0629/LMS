
#pragma once

#include <string_view>

namespace lms::api::subsonic
{
    enum class ResponseFormat
    {
        xml,
        json,
    };

    std::string_view ResponseFormatToMimeType(ResponseFormat format);
} // namespace lms::api::subsonic