
#pragma once

#include "core/Exception.hpp"

namespace lms::image
{
    class Exception : public core::LmsException
    {
    public:
        using LmsException::LmsException;
    };
} // namespace lms::image
