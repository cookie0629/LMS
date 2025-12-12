
#include "responses/ReplayGain.hpp"

#include "database/objects/Medium.hpp"
#include "database/objects/Track.hpp"

namespace lms::api::subsonic
{
    Response::Node createReplayGainNode(const db::Track::pointer& track, const db::Medium::pointer& medium)
    {
        Response::Node replayGainNode;

        if (const auto trackReplayGain{ track->getReplayGain() })
            replayGainNode.setAttribute("trackGain", *trackReplayGain);

        if (medium)
        {
            if (const auto releaseReplayGain{ medium->getReplayGain() })
                replayGainNode.setAttribute("albumGain", *releaseReplayGain);
        }

        return replayGainNode;
    }
} // namespace lms::api::subsonic