// 艺人评分实体的持久化实现

#include "database/objects/RatedArtist.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "database/Session.hpp"
#include "database/objects/Artist.hpp"
#include "database/objects/User.hpp"

#include "Utils.hpp"
#include "traits/IdTypeTraits.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::RatedArtist)

namespace lms::db
{
    RatedArtist::RatedArtist(ObjectPtr<Artist> artist, ObjectPtr<User> user)
        : _artist{ getDboPtr(artist) }
        , _user{ getDboPtr(user) }
    {
    }

    RatedArtist::pointer RatedArtist::create(Session& session, ObjectPtr<Artist> artist, ObjectPtr<User> user)
    {
        return session.getDboSession()->add(std::unique_ptr<RatedArtist>{ new RatedArtist{ artist, user } });
    }

    std::size_t RatedArtist::getCount(Session& session)
    {
        session.checkReadTransaction();
        return utils::fetchQuerySingleResult(session.getDboSession()->query<int>("SELECT COUNT(*) FROM rated_artist"));
    }

    RatedArtist::pointer RatedArtist::find(Session& session, RatedArtistId id)
    {
        session.checkReadTransaction();
        return utils::fetchQuerySingleResult(session.getDboSession()->find<RatedArtist>().where("id = ?").bind(id));
    }

    RatedArtist::pointer RatedArtist::find(Session& session, ArtistId artistId, UserId userId)
    {
        session.checkReadTransaction();
        return utils::fetchQuerySingleResult(session.getDboSession()->find<RatedArtist>().where("artist_id = ?").bind(artistId).where("user_id = ?").bind(userId));
    }

    void RatedArtist::find(Session& session, const FindParameters& params, std::function<void(const pointer&)> func)
    {
        session.checkReadTransaction();

        auto query{ session.getDboSession()->query<Wt::Dbo::ptr<RatedArtist>>("SELECT r_a FROM rated_artist r_a") };

        if (params.user.isValid())
            query.where("r_a.user_id = ?").bind(params.user);

        utils::forEachQueryRangeResult(query, params.range, func);
    }

    void RatedArtist::setLastUpdated(const Wt::WDateTime& lastUpdated)
    {
        _lastUpdated = utils::normalizeDateTime(lastUpdated);
    }
} // namespace lms::db
