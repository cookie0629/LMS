
#include "database/objects/StarredArtist.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "database/Session.hpp"
#include "database/objects/Artist.hpp"
#include "database/objects/User.hpp"

#include "Utils.hpp"
#include "traits/IdTypeTraits.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::StarredArtist)

namespace lms::db
{
    StarredArtist::StarredArtist(ObjectPtr<Artist> artist, ObjectPtr<User> user, FeedbackBackend backend)
        : _backend{ backend }
        , _artist{ getDboPtr(artist) }
        , _user{ getDboPtr(user) }
    {
    }

    StarredArtist::pointer StarredArtist::create(Session& session, ObjectPtr<Artist> artist, ObjectPtr<User> user, FeedbackBackend backend)
    {
        return session.getDboSession()->add(std::unique_ptr<StarredArtist>{ new StarredArtist{ artist, user, backend } });
    }

    std::size_t StarredArtist::getCount(Session& session)
    {
        session.checkReadTransaction();
        return utils::fetchQuerySingleResult(session.getDboSession()->query<int>("SELECT COUNT(*) FROM starred_artist"));
    }

    StarredArtist::pointer StarredArtist::find(Session& session, StarredArtistId id)
    {
        session.checkReadTransaction();
        return utils::fetchQuerySingleResult(session.getDboSession()->find<StarredArtist>().where("id = ?").bind(id));
    }

    StarredArtist::pointer StarredArtist::find(Session& session, ArtistId artistId, UserId userId)
    {
        session.checkReadTransaction();
        return utils::fetchQuerySingleResult(session.getDboSession()->query<Wt::Dbo::ptr<StarredArtist>>("SELECT s_a from starred_artist s_a").join("user u ON u.id = s_a.user_id").where("s_a.artist_id = ?").bind(artistId).where("s_a.user_id = ?").bind(userId).where("s_a.backend = u.feedback_backend"));
    }

    StarredArtist::pointer StarredArtist::find(Session& session, ArtistId artistId, UserId userId, FeedbackBackend backend)
    {
        session.checkReadTransaction();
        return utils::fetchQuerySingleResult(session.getDboSession()->find<StarredArtist>().where("artist_id = ?").bind(artistId).where("user_id = ?").bind(userId).where("backend = ?").bind(backend));
    }

    void StarredArtist::setDateTime(const Wt::WDateTime& dateTime)
    {
        _dateTime = utils::normalizeDateTime(dateTime);
    }
} // namespace lms::db
