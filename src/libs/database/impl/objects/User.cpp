#include "database/objects/User.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "database/Session.hpp"
#include "database/Utils.hpp"

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
        return session.getDboSession()->find<User>().resultList().size();
    }

    User::pointer User::find(Session& session, UserId id)
    {
        session.checkReadTransaction();
        Wt::Dbo::ptr<User> ptr = session.getDboSession()->find<User>()
            .where("id = ?")
            .bind(id.getValue());
        return ptr ? User::pointer(ptr) : User::pointer();
    }

    User::pointer User::find(Session& session, std::string_view loginName)
    {
        session.checkReadTransaction();
        Wt::Dbo::ptr<User> ptr = session.getDboSession()->find<User>()
            .where("login_name = ?")
            .bind(std::string{ loginName });
        return ptr ? User::pointer(ptr) : User::pointer();
    }

    User::pointer User::findDemoUser(Session& session)
    {
        session.checkReadTransaction();
        Wt::Dbo::ptr<User> ptr = session.getDboSession()->find<User>()
            .where("type = ?")
            .bind(static_cast<int>(UserType::DEMO));
        return ptr ? User::pointer(ptr) : User::pointer();
    }
} // namespace lms::db

