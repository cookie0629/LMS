#pragma once

#include <string_view>

#include <boost/asio/ip/address.hpp>

#include "database/objects/UserId.hpp"
#include "services/auth/Types.hpp"

namespace lms::db
{
    class IDb;
    class User;
} // namespace lms::db

namespace lms::auth
{
    /**
     * @brief 密码服务接口
     */
    class IPasswordService
    {
    public:
        virtual ~IPasswordService() = default;

        /**
         * @brief 密码检查结果
         */
        struct CheckResult
        {
            enum class State
            {
                Granted,   // 验证通过
                Denied,    // 验证失败
                Throttled, // 限流（防止暴力破解）
            };

            State state{ State::Denied };
            db::UserId userId{};
        };

        /**
         * @brief 密码可接受性检查结果
         */
        enum class PasswordAcceptabilityResult
        {
            OK,                    // 密码可用
            TooWeak,               // 密码太弱
            MustMatchLoginName,    // 密码不能与登录名相同
        };

        /**
         * @brief 检查用户密码
         * @param clientAddress 客户端IP地址
         * @param loginName 登录名
         * @param password 密码
         * @return 检查结果
         */
        virtual CheckResult checkUserPassword(
            const boost::asio::ip::address& clientAddress,
            std::string_view loginName,
            std::string_view password) = 0;

        /**
         * @brief 是否可以设置密码
         * @return true 如果可以设置密码，false 否则
         */
        virtual bool canSetPasswords() const = 0;

        /**
         * @brief 检查密码可接受性
         * @param password 密码
         * @param context 验证上下文
         * @return 检查结果
         */
        virtual PasswordAcceptabilityResult checkPasswordAcceptability(
            std::string_view password,
            const PasswordValidationContext& context) const = 0;

        /**
         * @brief 设置用户密码
         * @param userId 用户ID
         * @param newPassword 新密码
         */
        virtual void setPassword(db::UserId userId, std::string_view newPassword) = 0;
    };

    /**
     * @brief 创建密码服务
     * @param backend 后端类型（"internal", "pam" 等）
     * @param db 数据库引用
     * @param maxThrottlerEntryCount 限流器最大条目数
     * @return 密码服务实例
     */
    std::unique_ptr<IPasswordService> createPasswordService(
        std::string_view backend,
        db::IDb& db,
        std::size_t maxThrottlerEntryCount = 1000);
} // namespace lms::auth

