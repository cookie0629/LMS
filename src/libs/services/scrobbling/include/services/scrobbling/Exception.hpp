
#pragma once

#include "core/Exception.hpp"

namespace lms::scrobbling
{
    class Exception : public core::LmsException
    {
    public:
        using LmsException::LmsException;
    };
} // namespace lms::scrobbling
