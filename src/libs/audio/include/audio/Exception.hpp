
#pragma once

#include "core/Exception.hpp"

namespace lms::audio
{
    class Exception : public core::LmsException
    {
    public:
        using LmsException::LmsException;
    };
} // namespace lms::audio
