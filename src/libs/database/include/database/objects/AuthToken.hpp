
#pragma once

#include <optional>
#include <string_view>

#include <Wt/Dbo/Field.h>
#include <Wt/WDateTime.h>

#include "database/Object.hpp"
#include "database/objects/AuthTokenId.hpp"
#include "database/objects/UserId.hpp"

namespace lms::db
{
    class Session;
    class User;

    class AuthToken final : public Object<AuthToken, AuthTokenId>
    {
    public:
        AuthToken() = default;

        // Utility
        static std::size_t getCount(Session& session);
        static pointer find(Session& session, AuthTokenId tokenId);
        static pointer find(Session& session, std::string_view domain, std::string_view value);
        static void find(Session& session, std::string_view domain, UserId userId, std::function<void(const AuthToken::pointer&)> visitor);
        static void removeExpiredTokens(Session& session, std::string_view domain, const Wt::WDateTime& now);
        static void clearUserTokens(Session& session, std::string_view domain, UserId user);

        // Accessors
        const Wt::WDateTime& getExpiry() const { return _expiry; }
        ObjectPtr<User> getUser() const { return _user; }
        const std::string& getValue() const { return _value; }
        std::size_t getUseCount() const { return _useCount; }
        Wt::WDateTime getLastUsed() const { return _lastUsed; }
        std::optional<std::size_t> getMaxUseCount() const { return _maxUseCount; }

        // Setters
        std::size_t incUseCount() { return ++_useCount; }
        void setLastUsed(const Wt::WDateTime& lastUsed) { _lastUsed = lastUsed; }

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _domain, "domain");
            Wt::Dbo::field(a, _value, "value");
            Wt::Dbo::field(a, _expiry, "expiry");
            Wt::Dbo::field(a, _useCount, "use_count");
            Wt::Dbo::field(a, _lastUsed, "last_used");
            Wt::Dbo::field(a, _maxUseCount, "max_use_count");
            Wt::Dbo::belongsTo(a, _user, "user", Wt::Dbo::OnDeleteCascade);
        }

    private:
        friend class Session;
        AuthToken(std::string_view domain, std::string_view value, const Wt::WDateTime& expiry, std::optional<long> maxUseCount, ObjectPtr<User> user);
        static pointer create(Session& session, std::string_view domain, std::string_view value, const Wt::WDateTime& expiry, std::optional<long> maxUseCount, ObjectPtr<User> user);

        std::string _domain;
        std::string _value;
        Wt::WDateTime _expiry;
        long _useCount{};
        Wt::WDateTime _lastUsed;
        std::optional<long> _maxUseCount;
        Wt::Dbo::ptr<User> _user;
    };
} // namespace lms::db