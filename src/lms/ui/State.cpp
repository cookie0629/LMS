
#include "State.hpp"

#include "database/Session.hpp"
#include "database/objects/UIState.hpp"
#include "database/objects/User.hpp"

#include "LmsApplication.hpp"

namespace lms::ui::state::details
{
    void writeValue(std::string_view item, std::string_view value)
    {
        // No UI state stored for demo user
        if (LmsApp->getUserType() == db::UserType::DEMO)
            return;

        {
            auto transaction{ LmsApp->getDbSession().createWriteTransaction() };

            db::UIState::pointer state{ db::UIState::find(LmsApp->getDbSession(), item, LmsApp->getUserId()) };
            if (!state)
            {
                if (db::User::pointer user{ LmsApp->getUser() })
                    state = LmsApp->getDbSession().create<db::UIState>(item, user);
            }

            if (state)
                state.modify()->setValue(value);
        }
    }

    std::optional<std::string> readValue(std::string_view item)
    {
        std::optional<std::string> res;

        // No UI state stored for demo user
        if (LmsApp->getUserType() == db::UserType::DEMO)
            return res;

        {
            auto transaction{ LmsApp->getDbSession().createReadTransaction() };

            const db::UIState::pointer state{ db::UIState::find(LmsApp->getDbSession(), item, LmsApp->getUserId()) };
            if (state)
                res = state->getValue();
        }

        return res;
    }

    void eraseValue(std::string_view item)
    {
        // No UI state stored for demo user
        if (LmsApp->getUserType() == db::UserType::DEMO)
            return;

        {
            auto transaction{ LmsApp->getDbSession().createWriteTransaction() };

            db::UIState::pointer state{ db::UIState::find(LmsApp->getDbSession(), item, LmsApp->getUserId()) };
            if (state)
                state.remove();
        }
    }

} // namespace lms::ui::state::details
