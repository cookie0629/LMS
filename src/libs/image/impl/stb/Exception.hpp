
#pragma once

#include <string_view>

#include "image/Exception.hpp"

namespace lms::image
{
    class StbiException : public Exception
    {
    public:
        StbiException(std::string_view desc);
    };
} // namespace lms::image