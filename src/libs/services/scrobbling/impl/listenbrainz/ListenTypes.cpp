
#include "ListenTypes.hpp"

namespace lms::scrobbling::listenBrainz
{
    std::ostream& operator<<(std::ostream& os, const Listen& listen)
    {
        os << "track name = '" << listen.trackName << "', artistName = '" << listen.artistName << "'";
        if (listen.listenedAt.isValid())
            os << ", listenedAt = " << listen.listenedAt.toString();
        if (!listen.releaseName.empty())
            os << ", releaseName = '" << listen.releaseName << "'";
        if (listen.trackNumber)
            os << ", trackNumber = " << *listen.trackNumber;
        if (listen.trackMBID)
            os << ", trackMBID = '" << listen.trackMBID->getAsString() << "'";
        if (listen.recordingMBID)
            os << ", recordingMBID = '" << listen.recordingMBID->getAsString() << "'";

        return os;
    }
} // namespace lms::scrobbling::listenBrainz
