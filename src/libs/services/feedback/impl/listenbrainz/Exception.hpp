
#pragma once

#include "services/feedback/Exception.hpp"

namespace lms::feedback::listenBrainz
{
    class Exception : public feedback::Exception
    {
    public:
        using feedback::Exception::Exception;
    };
} // namespace lms::feedback::listenBrainz
