
#pragma once

#include "core/Exception.hpp"

namespace lms::podcast
{
    class Exception : public core::LmsException
    {
    public:
        using LmsException::LmsException;
    };
} // namespace lms::podcast
