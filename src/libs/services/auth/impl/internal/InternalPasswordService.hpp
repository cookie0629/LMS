
#pragma once

#include <Wt/Auth/HashFunction.h>
#include <Wt/Auth/PasswordStrengthValidator.h>

#include "database/objects/User.hpp"

#include "PasswordServiceBase.hpp"
#include "services/auth/IPasswordService.hpp"

namespace lms::auth
{
    class InternalPasswordService : public PasswordServiceBase
    {
    public:
        InternalPasswordService(db::IDb& db, std::size_t maxThrottlerEntries);

    private:
        bool checkUserPassword(std::string_view loginName, std::string_view password) override;

        bool canSetPasswords() const override;
        PasswordAcceptabilityResult checkPasswordAcceptability(std::string_view password, const PasswordValidationContext& context) const override;
        void setPassword(db::UserId userId, std::string_view newPassword) override;

        db::User::PasswordHash hashPassword(std::string_view password) const;
        void hashRandomPassword() const;

        const unsigned _bcryptRoundCount;
        const Wt::Auth::BCryptHashFunction _hashFunc{ static_cast<int>(_bcryptRoundCount) };
        Wt::Auth::PasswordStrengthValidator _validator;
    };
} // namespace lms::auth
