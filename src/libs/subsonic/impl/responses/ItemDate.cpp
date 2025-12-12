
#include "responses/Genre.hpp"

#include "database/objects/Cluster.hpp"

namespace lms::api::subsonic
{
    Response::Node createItemDateNode(const core::PartialDateTime& date)
    {
        Response::Node itemDateNode;

        if (auto year{ date.getYear() })
            itemDateNode.setAttribute("year", *year);
        if (auto month{ date.getMonth() })
            itemDateNode.setAttribute("month", *month);
        if (auto day{ date.getDay() })
            itemDateNode.setAttribute("day", *day);

        return itemDateNode;
    }
} // namespace lms::api::subsonic