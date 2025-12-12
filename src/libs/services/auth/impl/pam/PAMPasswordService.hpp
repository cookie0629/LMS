
#pragma once

#include "PasswordServiceBase.hpp"

namespace lms::auth
{
    class PAMPasswordService : public PasswordServiceBase
    {
    public:
        using PasswordServiceBase::PasswordServiceBase;

    private:
        bool checkUserPassword(std::string_view loginName, std::string_view password) override;
        bool canSetPasswords() const override;
        PasswordAcceptabilityResult checkPasswordAcceptability(std::string_view loginName, const PasswordValidationContext& context) const override;
        void setPassword(db::UserId userId, std::string_view newPassword) override;
    };
} // namespace lms::auth
