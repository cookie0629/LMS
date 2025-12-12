
#pragma once

#include <iosfwd>

#include <Wt/WDateTime.h>

#include "core/UUID.hpp"

namespace lms::feedback::listenBrainz
{
    // See https://listenbrainz.readthedocs.io/en/production/dev/feedback-json/#feedback-json-doc
    enum class FeedbackType
    {
        Love = 1,
        Hate = -1,
        Erase = 0,
    };

    struct Feedback
    {
        Wt::WDateTime created;
        core::UUID recordingMBID;
        FeedbackType score;
    };

    std::ostream& operator<<(std::ostream& os, const Feedback& feedback);

} // namespace lms::feedback::listenBrainz
