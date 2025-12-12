
#include "database/objects/TrackBookmark.hpp"

#include <Wt/Dbo/Impl.h>

#include "database/Session.hpp"
#include "database/objects/Track.hpp"
#include "database/objects/User.hpp"

#include "Utils.hpp"
#include "traits/IdTypeTraits.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::TrackBookmark)

namespace lms::db
{
    TrackBookmark::TrackBookmark(ObjectPtr<User> user, ObjectPtr<Track> track)
        : _user{ getDboPtr(user) }
        , _track{ getDboPtr(track) }
    {
    }

    TrackBookmark::pointer TrackBookmark::create(Session& session, ObjectPtr<User> user, ObjectPtr<Track> track)
    {
        return session.getDboSession()->add(std::unique_ptr<TrackBookmark>{ new TrackBookmark{ user, track } });
    }

    std::size_t TrackBookmark::getCount(Session& session)
    {
        session.checkReadTransaction();

        return utils::fetchQuerySingleResult(session.getDboSession()->query<int>("SELECT COUNT(*) FROM track_bookmark"));
    }

    RangeResults<TrackBookmarkId> TrackBookmark::find(Session& session, UserId userId, std::optional<Range> range)
    {
        session.checkReadTransaction();

        auto query{ session.getDboSession()->query<TrackBookmarkId>("SELECT id from track_bookmark").where("user_id = ?").bind(userId) };

        return utils::execRangeQuery<TrackBookmarkId>(query, range);
    }

    TrackBookmark::pointer TrackBookmark::find(Session& session, UserId userId, TrackId trackId)
    {
        session.checkReadTransaction();

        return utils::fetchQuerySingleResult(session.getDboSession()->find<TrackBookmark>().where("user_id = ?").bind(userId).where("track_id = ?").bind(trackId));
    }

    TrackBookmark::pointer TrackBookmark::find(Session& session, TrackBookmarkId id)
    {
        session.checkReadTransaction();

        return utils::fetchQuerySingleResult(session.getDboSession()->find<TrackBookmark>().where("id = ?").bind(id));
    }

} // namespace lms::db
