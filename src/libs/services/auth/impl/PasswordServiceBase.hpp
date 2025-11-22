#pragma once

#include <shared_mutex>

#include "AuthServiceBase.hpp"
#include "LoginThrottler.hpp"
#include "services/auth/IPasswordService.hpp"

namespace lms::db
{
    class IDb;
    class Session;
} // namespace lms::db

namespace lms::auth
{
    /**
     * @brief 密码服务基类
     * 提供限流和基础功能
     */
    class PasswordServiceBase : public IPasswordService, public AuthServiceBase
    {
    public:
        PasswordServiceBase(db::IDb& db, std::size_t maxThrottlerEntries);

        ~PasswordServiceBase() override = default;
        PasswordServiceBase(const PasswordServiceBase&) = delete;
        PasswordServiceBase& operator=(const PasswordServiceBase&) = delete;
        PasswordServiceBase(PasswordServiceBase&&) = delete;
        PasswordServiceBase& operator=(PasswordServiceBase&&) = delete;

    private:
        /**
         * @brief 检查用户密码（子类实现）
         * @param loginName 登录名
         * @param password 密码
         * @return true 如果密码正确，false 否则
         */
        virtual bool checkUserPassword(std::string_view loginName, std::string_view password) = 0;

        /**
         * @brief 检查用户密码（带限流）
         */
        CheckResult checkUserPassword(
            const boost::asio::ip::address& clientAddress,
            std::string_view loginName,
            std::string_view password) override;

        std::shared_mutex _mutex;
        LoginThrottler _loginThrottler;
    };
} // namespace lms::auth

