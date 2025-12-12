
#include "responses/Bookmark.hpp"

#include "database/objects/TrackBookmark.hpp"
#include "database/objects/User.hpp"

namespace lms::api::subsonic
{
    static const std::string_view reportedDummyDate{ "2000-01-01T00:00:00" };

    Response::Node createBookmarkNode(const db::ObjectPtr<db::TrackBookmark>& trackBookmark)
    {
        Response::Node trackBookmarkNode;

        trackBookmarkNode.setAttribute("position", trackBookmark->getOffset().count());
        if (!trackBookmark->getComment().empty())
            trackBookmarkNode.setAttribute("comment", trackBookmark->getComment());
        trackBookmarkNode.setAttribute("created", reportedDummyDate);
        trackBookmarkNode.setAttribute("changed", reportedDummyDate);
        trackBookmarkNode.setAttribute("username", trackBookmark->getUser()->getLoginName());

        return trackBookmarkNode;
    }
} // namespace lms::api::subsonic