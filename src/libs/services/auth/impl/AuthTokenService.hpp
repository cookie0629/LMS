#pragma once

#include <map>
#include <optional>
#include <shared_mutex>
#include <string_view>

#include "services/auth/IAuthTokenService.hpp"
#include "AuthServiceBase.hpp"
#include "LoginThrottler.hpp"

namespace lms::db
{
    class Session;
} // namespace lms::db

namespace lms::auth
{
    /**
     * @brief 认证令牌服务实现
     */
    class AuthTokenService : public IAuthTokenService, public AuthServiceBase
    {
    public:
        AuthTokenService(db::IDb& db, std::size_t maxThrottlerEntryCount);

        ~AuthTokenService() override = default;
        AuthTokenService(const AuthTokenService&) = delete;
        AuthTokenService& operator=(const AuthTokenService&) = delete;
        AuthTokenService(AuthTokenService&&) = delete;
        AuthTokenService& operator=(AuthTokenService&&) = delete;

    private:
        void registerDomain(std::string_view domain, const DomainParameters& params) override;
        AuthTokenProcessResult processAuthToken(
            std::string_view domain,
            const boost::asio::ip::address& clientAddress,
            std::string_view tokenValue) override;
        void visitAuthTokens(
            std::string_view domain,
            db::UserId userId,
            std::function<void(const AuthTokenInfo& info, std::string_view token)> visitor) override;
        void createAuthToken(
            std::string_view domain,
            db::UserId userId,
            std::string_view token) override;
        void clearAuthTokens(std::string_view domain, db::UserId userId) override;

        // 内部辅助方法
        std::optional<AuthTokenInfo> processAuthToken(std::string_view domain, std::string_view tokenValue);
        const DomainParameters& getDomainParameters(std::string_view domain) const;

        std::shared_mutex _mutex;
        std::map<std::string, DomainParameters> _domainParameters;
        LoginThrottler _loginThrottler;
    };
} // namespace lms::auth

