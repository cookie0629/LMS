
#include "responses/ItemGenre.hpp"

#include "database/objects/Cluster.hpp"

namespace lms::api::subsonic
{
    Response::Node createItemGenreNode(std::string_view name)
    {
        Response::Node genreNode;

        genreNode.setAttribute("name", name);

        return genreNode;
    }
} // namespace lms::api::subsonic