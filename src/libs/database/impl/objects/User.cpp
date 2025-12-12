
#include "database/objects/User.hpp"

#include <cassert>

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "database/Session.hpp"
#include "database/objects/Artist.hpp"
#include "database/objects/AuthToken.hpp"
#include "database/objects/Release.hpp"
#include "database/objects/Track.hpp"
#include "database/objects/UIState.hpp"

#include "Utils.hpp"
#include "traits/EnumSetTraits.hpp"
#include "traits/IdTypeTraits.hpp"
#include "traits/StringViewTraits.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::User)

namespace lms::db
{
    User::User(std::string_view loginName)
        : _loginName{ loginName }
    {
    }

    User::pointer User::create(Session& session, std::string_view loginName)
    {
        return session.getDboSession()->add(std::unique_ptr<User>{ new User{ loginName } });
    }

    std::size_t User::getCount(Session& session)
    {
        session.checkReadTransaction();

        return utils::fetchQuerySingleResult(session.getDboSession()->query<int>("SELECT COUNT(*) FROM user"));
    }

    RangeResults<UserId> User::find(Session& session, const FindParameters& params)
    {
        session.checkReadTransaction();

        auto query{ session.getDboSession()->query<UserId>("SELECT id FROM user") };

        if (params.scrobblingBackend)
            query.where("scrobbling_backend = ?").bind(*params.scrobblingBackend);
        if (params.feedbackBackend)
            query.where("feedback_backend = ?").bind(*params.feedbackBackend);

        return utils::execRangeQuery<UserId>(query, params.range);
    }

    void User::find(Session& session, const FindParameters& params, const std::function<void(const User::pointer&)>& func)
    {
        auto query{ session.getDboSession()->find<User>() };

        if (params.scrobblingBackend)
            query.where("scrobbling_backend = ?").bind(*params.scrobblingBackend);
        if (params.feedbackBackend)
            query.where("feedback_backend = ?").bind(*params.feedbackBackend);

        return utils::forEachQueryRangeResult(query, params.range, func);
    }

    User::pointer User::findDemoUser(Session& session)
    {
        session.checkReadTransaction();

        return utils::fetchQuerySingleResult(session.getDboSession()->query<Wt::Dbo::ptr<User>>("SELECT u from user u").where("u.type = ?").bind(UserType::DEMO));
    }

    User::pointer User::find(Session& session, UserId id)
    {
        return utils::fetchQuerySingleResult(session.getDboSession()->query<Wt::Dbo::ptr<User>>("SELECT u from user u").where("u.id = ?").bind(id));
    }

    User::pointer User::find(Session& session, std::string_view name)
    {
        return utils::fetchQuerySingleResult(session.getDboSession()->query<Wt::Dbo::ptr<User>>("SELECT u from user u").where("u.login_name = ?").bind(name));
    }

    void User::setSubsonicDefaultTranscodingOutputBitrate(Bitrate bitrate)
    {
        assert(isAudioBitrateAllowed(bitrate));
        _subsonicDefaultTranscodingOutputBitrate = bitrate;
    }
} // namespace lms::db
