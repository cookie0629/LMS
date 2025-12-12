
#pragma once

#include <chrono>
#include <memory>
#include <optional>
#include <string_view>

#include <Wt/WDateTime.h>
#include <boost/asio/ip/address.hpp>

#include "core/LiteralString.hpp"
#include "database/objects/UserId.hpp"

namespace lms::db
{
    class IDb;
} // namespace lms::db

namespace lms::auth
{
    class IAuthTokenService
    {
    public:
        virtual ~IAuthTokenService() = default;

        struct AuthTokenInfo
        {
            db::UserId userId;
            Wt::WDateTime expiry;
            Wt::WDateTime lastUsed; // if called by processAuthToken, value is before processing
            std::size_t useCount;   // if called by processAuthToken, value is before processing
            std::optional<std::size_t> maxUseCount;
        };

        struct AuthTokenProcessResult
        {
            enum class State
            {
                Granted,
                Throttled,
                Denied,
            };

            State state{ State::Denied };
            std::optional<AuthTokenInfo> authTokenInfo;
        };

        struct DomainParameters
        {
            std::optional<std::size_t> tokenMaxUseCount;
            std::optional<std::chrono::seconds> tokenDuration;
        };

        virtual void registerDomain(core::LiteralString domain, const DomainParameters& params) = 0;

        // Processing an auth token will make its useCount increase by 1. Token is then automatically deleted if its maxUsecount is reached
        virtual AuthTokenProcessResult processAuthToken(core::LiteralString domain, const boost::asio::ip::address& clientAddress, std::string_view tokenValue) = 0;

        virtual void visitAuthTokens(core::LiteralString domain, db::UserId userid, std::function<void(const AuthTokenInfo& info, std::string_view token)> visitor) = 0;

        virtual void createAuthToken(core::LiteralString domain, db::UserId userid, std::string_view token) = 0;
        virtual void clearAuthTokens(core::LiteralString domain, db::UserId userid) = 0;
    };

    std::unique_ptr<IAuthTokenService> createAuthTokenService(db::IDb& db, std::size_t maxThrottlerEntryCount);
} // namespace lms::auth
