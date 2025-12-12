
#include "Playlist.hpp"

#include "core/Service.hpp"
#include "core/String.hpp"
#include "database/objects/Artwork.hpp"
#include "database/objects/TrackList.hpp"
#include "database/objects/User.hpp"
#include "services/artwork/IArtworkService.hpp"

#include "CoverArtId.hpp"
#include "RequestContext.hpp"
#include "SubsonicId.hpp"

namespace lms::api::subsonic
{
    Response::Node createPlaylistNode(RequestContext& context, const db::TrackList::pointer& tracklist)
    {
        Response::Node playlistNode;

        playlistNode.setAttribute("id", idToString(tracklist->getId()));
        playlistNode.setAttribute("name", tracklist->getName());
        playlistNode.setAttribute("songCount", tracklist->getCount());
        playlistNode.setAttribute("duration", std::chrono::duration_cast<std::chrono::seconds>(tracklist->getDuration()).count());
        playlistNode.setAttribute("public", tracklist->getVisibility() == db::TrackList::Visibility::Public);
        playlistNode.setAttribute("changed", core::stringUtils::toISO8601String(tracklist->getLastModifiedDateTime()));
        playlistNode.setAttribute("created", core::stringUtils::toISO8601String(tracklist->getCreationDateTime()));
        if (const db::User::pointer user{ tracklist->getUser() })
            playlistNode.setAttribute("owner", user->getLoginName());

        if (const db::ArtworkId artworkId{ core::Service<artwork::IArtworkService>::get()->findTrackListImage(tracklist->getId()) }; artworkId.isValid())
        {
            if (const auto artwork{ db::Artwork::find(context.getDbSession(), artworkId) })
            {
                CoverArtId coverArtId{ artwork->getId(), artwork->getLastWrittenTime().toTime_t() };
                playlistNode.setAttribute("coverArt", idToString(coverArtId));
            }
        }

        return playlistNode;
    }
} // namespace lms::api::subsonic