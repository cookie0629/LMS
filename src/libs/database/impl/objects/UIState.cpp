
#include "database/objects/UIState.hpp"

#include <Wt/Dbo/Impl.h>

#include "database/Session.hpp"
#include "database/objects/User.hpp"

#include "Utils.hpp"
#include "traits/IdTypeTraits.hpp"
#include "traits/StringViewTraits.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::UIState)

namespace lms::db
{
    UIState::UIState(std::string_view item, ObjectPtr<User> user)
        : _item{ item }
        , _user{ getDboPtr(user) }
    {
    }

    UIState::pointer UIState::create(Session& session, std::string_view item, ObjectPtr<User> user)
    {
        return session.getDboSession()->add(std::unique_ptr<UIState>{ new UIState{ item, user } });
    }

    std::size_t UIState::getCount(Session& session)
    {
        session.checkReadTransaction();

        return utils::fetchQuerySingleResult(session.getDboSession()->query<int>("SELECT COUNT(*) FROM ui_state"));
    }

    UIState::pointer UIState::find(Session& session, UIStateId settingId)
    {
        auto query{ session.getDboSession()->query<Wt::Dbo::ptr<UIState>>("SELECT ui_s from ui_state ui_s").where("ui_s.id = ?").bind(settingId) };
        return utils::fetchQuerySingleResult(query);
    }

    UIState::pointer UIState::find(Session& session, std::string_view item, UserId userId)
    {
        auto query{ session.getDboSession()->query<Wt::Dbo::ptr<UIState>>("SELECT ui_s from ui_state ui_s").where("ui_s.item = ?").bind(item).where("ui_s.user_id = ?").bind(userId) };
        return utils::fetchQuerySingleResult(query);
    }
} // namespace lms::db