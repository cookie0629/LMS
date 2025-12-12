
#include "FeedbackTypes.hpp"

#include <ostream>

namespace lms::feedback::listenBrainz
{
    std::ostream& operator<<(std::ostream& os, const Feedback& feedback)
    {
        os << "created = '" << feedback.created.toString() << "', recording MBID = '" << feedback.recordingMBID.getAsString() << "', score = " << static_cast<int>(feedback.score);
        return os;
    }
} // namespace lms::feedback::listenBrainz
