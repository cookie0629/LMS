#include "AuthTokenService.hpp"

#include <boost/asio/ip/address.hpp>

#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/AuthToken.hpp"
#include "database/objects/User.hpp"
#include "services/auth/Types.hpp"

namespace lms::auth
{
    namespace
    {
        AuthTokenService::AuthTokenInfo createAuthTokenInfo(const db::AuthToken::pointer& authToken)
        {
            return AuthTokenService::AuthTokenInfo{
                .userId = authToken->getUser()->getId(),
                .expiry = authToken->getExpiry(),
                .lastUsed = authToken->getLastUsed(),
                .useCount = authToken->getUseCount(),
                .maxUseCount = authToken->getMaxUseCount(),
            };
        }
    } // namespace

    std::unique_ptr<IAuthTokenService> createAuthTokenService(db::IDb& db, std::size_t maxThrottlerEntryCount)
    {
        return std::make_unique<AuthTokenService>(db, maxThrottlerEntryCount);
    }

    AuthTokenService::AuthTokenService(db::IDb& db, std::size_t maxThrottlerEntryCount)
        : AuthServiceBase{ db }
        , _loginThrottler{ maxThrottlerEntryCount }
    {
    }

    void AuthTokenService::registerDomain(std::string_view domain, const DomainParameters& params)
    {
        std::unique_lock lock(_mutex);
        auto [it, inserted] = _domainParameters.emplace(std::string{ domain }, params);
        if (!inserted)
            throw Exception{ "Auth token domain already registered!" };
    }

    void AuthTokenService::createAuthToken(
        std::string_view domain,
        db::UserId userId,
        std::string_view token)
    {
        const DomainParameters& params{ getDomainParameters(domain) };

        db::Session& session{ getDbSession() };
        const auto now{ Wt::WDateTime::currentDateTime() };
        const auto expiry{ params.tokenDuration ? now.addSecs(std::chrono::duration_cast<std::chrono::seconds>(params.tokenDuration.value()).count()) : Wt::WDateTime{} };

        {
            auto transaction{ session.createWriteTransaction() };

            const db::User::pointer user{ db::User::find(session, userId) };
            if (!user)
                throw Exception{ "User deleted" };

            std::optional<long> maxUseCount;
            if (params.tokenMaxUseCount)
                maxUseCount = static_cast<long>(*params.tokenMaxUseCount);

            const db::AuthToken::pointer authToken{ session.create<db::AuthToken>(
                std::string{ domain }, 
                std::string{ token }, 
                expiry, 
                maxUseCount, 
                user) };

            LMS_LOG(UI, DEBUG, "Created auth token for user '" << user->getLoginName() 
                << "', expiry = " << (authToken->getExpiry().isValid() ? authToken->getExpiry().toString() : "<unset>")
                << ", maxUseCount = " << (authToken->getMaxUseCount() ? std::to_string(*authToken->getMaxUseCount()) : "<unset>"));

            // 如果用户令牌数量过多，清理过期令牌
            if (user->getAuthTokensCount() >= 50)
                db::AuthToken::removeExpiredTokens(session, std::string{ domain }, Wt::WDateTime::currentDateTime());

            // 显式提交事务，确保数据被保存
            transaction.commit();
        }
    }

    std::optional<AuthTokenService::AuthTokenInfo> AuthTokenService::processAuthToken(std::string_view domain, std::string_view tokenValue)
    {
        db::Session& session{ getDbSession() };
        auto transaction{ session.createWriteTransaction() };

        db::AuthToken::pointer authToken{ db::AuthToken::find(session, std::string{ domain }, std::string{ tokenValue }) };
        if (!authToken)
        {
            LMS_LOG(UI, DEBUG, "Auth token not found for domain '" << domain << "', token '" << tokenValue << "'");
            return std::nullopt;
        }

        if (authToken->getExpiry().isValid() && authToken->getExpiry() < Wt::WDateTime::currentDateTime())
        {
            authToken.remove();
            return std::nullopt;
        }

        LMS_LOG(UI, DEBUG, "Found auth token for user '" << authToken->getUser()->getLoginName() << "' on domain '" << domain << "'");

        AuthTokenInfo res{ createAuthTokenInfo(authToken) };

        const std::size_t tokenUseCount{ authToken.modify()->incUseCount() };
        authToken.modify()->setLastUsed(Wt::WDateTime::currentDateTime());

        if (auto maxUseCount{ authToken->getMaxUseCount() })
        {
            if (*maxUseCount <= tokenUseCount)
                authToken.remove();
        }

        return res;
    }

    IAuthTokenService::AuthTokenProcessResult AuthTokenService::processAuthToken(
        std::string_view domain,
        const boost::asio::ip::address& clientAddress,
        std::string_view tokenValue)
    {
        // 先检查是否被限流（使用共享锁，避免浪费资源）
        {
            std::shared_lock lock{ _mutex };

            if (_loginThrottler.isClientThrottled(clientAddress))
                return AuthTokenProcessResult{ .state = AuthTokenProcessResult::State::Throttled, .authTokenInfo = std::nullopt };
        }

        // 处理令牌
        auto res{ processAuthToken(domain, tokenValue) };
        
        {
            std::unique_lock lock{ _mutex };

            // 再次检查限流（可能在处理过程中被限流）
            if (_loginThrottler.isClientThrottled(clientAddress))
                return AuthTokenProcessResult{ .state = AuthTokenProcessResult::State::Throttled, .authTokenInfo = std::nullopt };

            if (!res)
            {
                _loginThrottler.onBadClientAttempt(clientAddress);
                return AuthTokenProcessResult{ .state = AuthTokenProcessResult::State::Denied, .authTokenInfo = std::nullopt };
            }

            _loginThrottler.onGoodClientAttempt(clientAddress);
            onUserAuthenticated(res->userId);
            return AuthTokenProcessResult{ .state = AuthTokenProcessResult::State::Granted, .authTokenInfo = res };
        }
    }

    void AuthTokenService::visitAuthTokens(
        std::string_view domain,
        db::UserId userId,
        std::function<void(const AuthTokenInfo& info, std::string_view token)> visitor)
    {
        db::Session& session{ getDbSession() };

        {
            auto transaction{ session.createReadTransaction() };

            db::AuthToken::find(session, std::string{ domain }, userId, [&](const db::AuthToken::pointer& authToken) {
                const AuthTokenInfo info{ createAuthTokenInfo(authToken) };
                visitor(info, authToken->getValue());
            });
        }
    }

    void AuthTokenService::clearAuthTokens(std::string_view domain, db::UserId userId)
    {
        db::Session& session{ getDbSession() };

        {
            auto transaction{ session.createWriteTransaction() };
            db::AuthToken::clearUserTokens(session, std::string{ domain }, userId);
        }
    }

    const AuthTokenService::DomainParameters& AuthTokenService::getDomainParameters(std::string_view domain) const
    {
        auto it{ _domainParameters.find(std::string{ domain }) };
        if (it == std::cend(_domainParameters))
            throw Exception{ "Invalid auth token domain" };

        return it->second;
    }
} // namespace lms::auth
