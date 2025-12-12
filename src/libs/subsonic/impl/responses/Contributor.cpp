
#include "responses/Contributor.hpp"

#include "database/Object.hpp"
#include "database/objects/TrackArtistLink.hpp"

#include "SubsonicResponse.hpp"
#include "responses/Artist.hpp"

namespace lms::api::subsonic
{
    Response::Node createContributorNode(const db::ObjectPtr<db::TrackArtistLink>& trackArtistLink, const db::ObjectPtr<db::Artist>& artist)
    {
        Response::Node contributorNode;

        contributorNode.setAttribute("role", utils::toString(trackArtistLink->getType()));
        if (!trackArtistLink->getSubType().empty())
            contributorNode.setAttribute("subRole", trackArtistLink->getSubType());
        contributorNode.addChild("artist", createArtistNode(artist));

        return contributorNode;
    }
} // namespace lms::api::subsonic