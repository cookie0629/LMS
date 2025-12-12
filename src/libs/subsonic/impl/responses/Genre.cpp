
#include "responses/Genre.hpp"

#include "database/objects/Cluster.hpp"

#include "RequestContext.hpp"

namespace lms::api::subsonic
{
    Response::Node createGenreNode(RequestContext& context, const db::Cluster::pointer& cluster)
    {
        Response::Node clusterNode;

        switch (context.getResponseFormat())
        {
        case ResponseFormat::json:
            clusterNode.setAttribute("value", cluster->getName());
            break;
        case ResponseFormat::xml:
            clusterNode.setValue(cluster->getName());
            break;
        }
        clusterNode.setAttribute("songCount", cluster->getTrackCount());
        clusterNode.setAttribute("albumCount", cluster->getReleasesCount());

        return clusterNode;
    }
} // namespace lms::api::subsonic