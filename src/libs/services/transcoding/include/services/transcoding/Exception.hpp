
#pragma once

#include "core/Exception.hpp"

namespace lms::transcoding
{
    class Exception : public core::LmsException
    {
    public:
        using LmsException::LmsException;
    };
} // namespace lms::transcoding