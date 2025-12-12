
#pragma once

#include "services/scrobbling/Exception.hpp"

namespace lms::scrobbling::listenBrainz
{
    class Exception : public scrobbling::Exception
    {
    public:
        using scrobbling::Exception::Exception;
    };
} // namespace lms::scrobbling::listenBrainz
