
#pragma once

#include "core/Exception.hpp"

namespace lms::feedback
{
    class Exception : public core::LmsException
    {
    public:
        using LmsException::LmsException;
    };
} // namespace lms::feedback
