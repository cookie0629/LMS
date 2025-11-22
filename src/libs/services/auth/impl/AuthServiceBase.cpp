#include "AuthServiceBase.hpp"

#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/User.hpp"

namespace lms::auth
{
    AuthServiceBase::AuthServiceBase(db::IDb& db)
        : _db{ db }
    {
    }

    db::Session& AuthServiceBase::getDbSession()
    {
        return _db.getTLSSession();
    }

    db::UserId AuthServiceBase::getOrCreateUser(std::string_view loginName)
    {
        auto& session = getDbSession();
        auto transaction = session.createWriteTransaction();

        auto user = db::User::find(session, loginName);
        if (!user)
        {
            // 创建新用户
            user = session.create<db::User>(loginName);
        }

        // 获取用户ID（通过 User 的 getId() 方法）
        return user->getId();
    }

    void AuthServiceBase::onUserAuthenticated(db::UserId userId)
    {
        auto& session = getDbSession();
        auto transaction = session.createWriteTransaction();

        auto user = db::User::find(session, userId);
        if (user)
        {
            // 更新最后登录时间
            user.modify()->setLastLogin(Wt::WDateTime::currentDateTime());
        }
    }
} // namespace lms::auth

