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
    /**
     * @brief Auth Service 基类
     * 提供数据库会话访问和用户管理功能
     */
    class AuthServiceBase
    {
    protected:
        explicit AuthServiceBase(db::IDb& db);
        ~AuthServiceBase() = default;
        AuthServiceBase(const AuthServiceBase&) = delete;
        AuthServiceBase& operator=(const AuthServiceBase&) = delete;

        /**
         * @brief 获取或创建用户
         * @param loginName 登录名
         * @return 用户ID
         */
        db::UserId getOrCreateUser(std::string_view loginName);

        /**
         * @brief 用户认证成功后的回调
         * @param userId 用户ID
         */
        void onUserAuthenticated(db::UserId userId);

        /**
         * @brief 获取数据库会话
         * @return 数据库会话引用
         */
        db::Session& getDbSession();

    private:
        db::IDb& _db;
    };
} // namespace lms::auth

