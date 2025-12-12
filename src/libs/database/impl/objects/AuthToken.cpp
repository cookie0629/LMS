// 登录 token 持久化与查询实现

#include "database/objects/AuthToken.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "database/Session.hpp"
#include "database/objects/User.hpp"

#include "Utils.hpp"
#include "traits/IdTypeTraits.hpp"
#include "traits/StringViewTraits.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::AuthToken)

namespace lms::db
{
    AuthToken::AuthToken(std::string_view domain, std::string_view value, const Wt::WDateTime& expiry, std::optional<long> maxUseCount, ObjectPtr<User> user)
        : _domain{ domain }
        , _value{ value }
        , _expiry{ expiry }
        , _maxUseCount{ maxUseCount }
        , _user{ getDboPtr(user) }
    {
    }

    AuthToken::pointer AuthToken::create(Session& session, std::string_view domain, std::string_view value, const Wt::WDateTime& expiry, std::optional<long> maxUseCount, ObjectPtr<User> user)
    {
        return session.getDboSession()->add(std::unique_ptr<AuthToken>{ new AuthToken{ domain, value, expiry, maxUseCount, user } });
    }

    std::size_t AuthToken::getCount(Session& session)
    {
        session.checkReadTransaction();

        return utils::fetchQuerySingleResult(session.getDboSession()->query<int>("SELECT COUNT(*) FROM auth_token"));
    }

    AuthToken::pointer AuthToken::find(Session& session, AuthTokenId id)
    {
        return utils::fetchQuerySingleResult(session.getDboSession()->query<Wt::Dbo::ptr<AuthToken>>("SELECT a_t from auth_token a_t").where("a_t.id = ?").bind(id));
    }

    AuthToken::pointer AuthToken::find(Session& session, std::string_view domain, std::string_view value)
    {
        session.checkReadTransaction();

        auto query{ session.getDboSession()->find<AuthToken>() };
        query.where("domain = ?").bind(domain);
        query.where("value = ?").bind(value);

        return utils::fetchQuerySingleResult(query);
    }

    void AuthToken::find(Session& session, std::string_view domain, UserId userId, std::function<void(const AuthToken::pointer&)> visitor)
    {
        session.checkReadTransaction();

        auto query{ session.getDboSession()->find<AuthToken>() };
        query.where("domain = ?").bind(domain);
        query.where("user_id = ?").bind(userId);

        utils::forEachQueryResult(query, visitor);
    }

    void AuthToken::removeExpiredTokens(Session& session, std::string_view domain, const Wt::WDateTime& now)
    {
        session.checkWriteTransaction();

        utils::executeCommand(*session.getDboSession(), "DELETE FROM auth_token WHERE expiry < ? AND domain = ?", now, domain);
    }

    void AuthToken::clearUserTokens(Session& session, std::string_view domain, UserId user)
    {
        session.checkWriteTransaction();

        utils::executeCommand(*session.getDboSession(), "DELETE FROM auth_token WHERE user_id = ? AND domain = ?", user, domain);
    }
} // namespace lms::db
