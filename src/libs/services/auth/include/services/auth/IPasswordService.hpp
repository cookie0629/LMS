
#pragma once

#include <string_view>

#include <Wt/Dbo/ptr.h>
#include <Wt/WDateTime.h>
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
    class IPasswordService
    {
    public:
        virtual ~IPasswordService() = default;

        struct CheckResult
        {
            enum class State
            {
                Granted,
                Denied,
                Throttled,
            };
            State state{ State::Denied };
            db::UserId userId{};
        };
        virtual CheckResult checkUserPassword(const boost::asio::ip::address& clientAddress,
                                              std::string_view loginName,
                                              std::string_view password)
            = 0;

        virtual bool canSetPasswords() const = 0;

        enum class PasswordAcceptabilityResult
        {
            OK,
            TooWeak,
            MustMatchLoginName,
        };
        virtual PasswordAcceptabilityResult checkPasswordAcceptability(std::string_view password, const PasswordValidationContext& context) const = 0;
        virtual void setPassword(db::UserId userId, std::string_view newPassword) = 0;
    };

    std::unique_ptr<IPasswordService> createPasswordService(std::string_view backend, db::IDb& db, std::size_t maxThrottlerEntryCount);
} // namespace lms::auth
