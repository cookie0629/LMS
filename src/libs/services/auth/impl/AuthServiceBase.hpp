
#pragma once

#include <string_view>

#include "database/objects/UserId.hpp"

namespace lms::db
{
    class IDb;
    class Session;
} // namespace lms::db

namespace lms::auth
{
    class AuthServiceBase
    {
    protected:
        AuthServiceBase(db::IDb& db);
        ~AuthServiceBase() = default;
        AuthServiceBase(const AuthServiceBase&) = delete;
        AuthServiceBase& operator=(const AuthServiceBase&) = delete;

        db::UserId getOrCreateUser(std::string_view loginName);
        void onUserAuthenticated(db::UserId userId);

        db::Session& getDbSession();

    private:
        db::IDb& _db;
    };
} // namespace lms::auth
