
#pragma once

#include <string>
#include <string_view>

#include <Wt/Dbo/Field.h>

#include "database/Object.hpp"
#include "database/objects/UIStateId.hpp"
#include "database/objects/UserId.hpp"

namespace lms::db
{
    class User;
    class Session;

    class UIState final : public Object<UIState, UIStateId>
    {
    public:
        UIState() = default;

        static std::size_t getCount(Session& session);
        static pointer find(Session& session, UIStateId settingId);
        static pointer find(Session& session, std::string_view item, UserId userId);

        // Getters
        std::string_view getValue() const { return _value; };

        // Setters
        void setValue(std::string_view value) { _value = value; };

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _item, "item");
            Wt::Dbo::field(a, _value, "value");

            Wt::Dbo::belongsTo(a, _user, "user", Wt::Dbo::OnDeleteCascade);
        }

    private:
        friend class Session;
        UIState(std::string_view item, ObjectPtr<User> user);
        static pointer create(Session& session, std::string_view item, ObjectPtr<User> user);

        std::string _item;
        std::string _value;
        Wt::Dbo::ptr<User> _user;
    };
} // namespace lms::db
