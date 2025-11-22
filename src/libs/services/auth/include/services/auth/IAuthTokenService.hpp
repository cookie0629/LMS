#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string_view>

#include <Wt/WDateTime.h>
#include <boost/asio/ip/address.hpp>

#include "database/objects/UserId.hpp"

namespace lms::db
{
    class IDb;
} // namespace lms::db

namespace lms::auth
{
    /**
     * @brief 认证令牌服务接口
     */
    class IAuthTokenService
    {
    public:
        virtual ~IAuthTokenService() = default;

        /**
         * @brief 认证令牌信息
         */
        struct AuthTokenInfo
        {
            db::UserId userId;
            Wt::WDateTime expiry;
            Wt::WDateTime lastUsed;
            std::size_t useCount;
            std::optional<std::size_t> maxUseCount;
        };

        /**
         * @brief 认证令牌处理结果
         */
        struct AuthTokenProcessResult
        {
            enum class State
            {
                Granted,   // 授权通过
                Throttled, // 限流
                Denied,    // 拒绝
            };

            State state{ State::Denied };
            std::optional<AuthTokenInfo> authTokenInfo;
        };

        /**
         * @brief 域参数
         */
        struct DomainParameters
        {
            std::optional<std::size_t> tokenMaxUseCount;
            std::optional<std::chrono::seconds> tokenDuration;
        };

        /**
         * @brief 注册域
         * @param domain 域名
         * @param params 域参数
         */
        virtual void registerDomain(std::string_view domain, const DomainParameters& params) = 0;

        /**
         * @brief 处理认证令牌
         * @param domain 域名
         * @param clientAddress 客户端IP地址
         * @param tokenValue 令牌值
         * @return 处理结果
         */
        virtual AuthTokenProcessResult processAuthToken(
            std::string_view domain,
            const boost::asio::ip::address& clientAddress,
            std::string_view tokenValue) = 0;

        /**
         * @brief 访问认证令牌
         * @param domain 域名
         * @param userId 用户ID
         * @param visitor 访问器函数
         */
        virtual void visitAuthTokens(
            std::string_view domain,
            db::UserId userId,
            std::function<void(const AuthTokenInfo& info, std::string_view token)> visitor) = 0;

        /**
         * @brief 创建认证令牌
         * @param domain 域名
         * @param userId 用户ID
         * @param token 令牌值
         */
        virtual void createAuthToken(
            std::string_view domain,
            db::UserId userId,
            std::string_view token) = 0;

        /**
         * @brief 清除认证令牌
         * @param domain 域名
         * @param userId 用户ID
         */
        virtual void clearAuthTokens(std::string_view domain, db::UserId userId) = 0;
    };

    /**
     * @brief 创建认证令牌服务
     * @param db 数据库引用
     * @param maxThrottlerEntryCount 限流器最大条目数
     * @return 认证令牌服务实例
     */
    std::unique_ptr<IAuthTokenService> createAuthTokenService(
        db::IDb& db,
        std::size_t maxThrottlerEntryCount = 1000);
} // namespace lms::auth

