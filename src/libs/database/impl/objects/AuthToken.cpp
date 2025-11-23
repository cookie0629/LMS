#include "database/objects/AuthToken.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "database/Session.hpp"
#include "database/objects/User.hpp"
#include "database/Utils.hpp"

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

        auto query = session.getDboSession()->query<int>("SELECT COUNT(*) FROM auth_token");
        auto result = utils::fetchQuerySingleResult(query);
        return result ? *result : 0;
    }

    AuthToken::pointer AuthToken::find(Session& session, AuthTokenId id)
    {
        session.checkReadTransaction();
        
        auto query = session.getDboSession()->query<Wt::Dbo::ptr<AuthToken>>("SELECT a_t from auth_token a_t").where("a_t.id = ?").bind(id.getValue());
        auto result = utils::fetchQuerySingleResult(query);
        return result ? *result : pointer{};
    }

    AuthToken::pointer AuthToken::find(Session& session, std::string_view domain, std::string_view value)
    {
        session.checkReadTransaction();

        auto query{ session.getDboSession()->find<AuthToken>() };
        query.where("domain = ?").bind(std::string{ domain });
        query.where("value = ?").bind(std::string{ value });

        auto result = utils::fetchQuerySingleResult(query);
        return result ? *result : pointer{};
    }

    void AuthToken::find(Session& session, std::string_view domain, UserId userId, std::function<void(const AuthToken::pointer&)> visitor)
    {
        session.checkReadTransaction();

        auto query{ session.getDboSession()->find<AuthToken>() };
        query.where("domain = ?").bind(std::string{ domain });
        query.where("user_id = ?").bind(userId.getValue());

        auto results = query.resultList();
        for (const auto& result : results)
        {
            visitor(result);
        }
    }

    void AuthToken::removeExpiredTokens(Session& session, std::string_view domain, const Wt::WDateTime& now)
    {
        session.checkWriteTransaction();

        // 使用 Wt::Dbo 的查询接口删除过期令牌
        auto query = session.getDboSession()->query<int>("DELETE FROM auth_token WHERE expiry < ? AND domain = ?");
        query.bind(now);
        query.bind(std::string{ domain });
        // 执行查询（返回受影响的行数，这里不需要使用）
        (void)query.resultList();
    }

    void AuthToken::clearUserTokens(Session& session, std::string_view domain, UserId userId)
    {
        session.checkWriteTransaction();

        // 使用 Wt::Dbo 的查询接口删除用户令牌
        auto query = session.getDboSession()->query<int>("DELETE FROM auth_token WHERE user_id = ? AND domain = ?");
        query.bind(userId.getValue());
        query.bind(std::string{ domain });
        // 执行查询（返回受影响的行数，这里不需要使用）
        (void)query.resultList();
    }
} // namespace lms::db

