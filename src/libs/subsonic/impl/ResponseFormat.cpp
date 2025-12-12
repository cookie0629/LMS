
#include "ResponseFormat.hpp"

#include <string_view>

namespace lms::api::subsonic
{
    std::string_view ResponseFormatToMimeType(ResponseFormat format)
    {
        switch (format)
        {
        case ResponseFormat::xml:
            return "text/xml";
        case ResponseFormat::json:
            return "application/json";
        }

        return "";
    }
} // namespace lms::api::subsonic