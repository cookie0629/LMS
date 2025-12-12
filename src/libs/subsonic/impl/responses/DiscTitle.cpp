
#include "responses/DiscTitle.hpp"

#include "database/objects/Medium.hpp"

namespace lms::api::subsonic
{
    Response::Node createDiscTitle(const db::ObjectPtr<db::Medium>& medium)
    {
        Response::Node discTitleNode;

        discTitleNode.setAttribute("disc", medium->getPosition() ? *medium->getPosition() : 0);
        discTitleNode.setAttribute("title", medium->getName());

        return discTitleNode;
    }
} // namespace lms::api::subsonic