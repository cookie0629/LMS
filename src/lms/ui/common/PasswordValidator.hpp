
#pragma once

#include <functional>

#include <Wt/WValidator.h>

#include "services/auth/Types.hpp"

namespace lms::auth
{
    class IPasswordService;
}

namespace lms::ui
{
    using PasswordValidationContextGetFunc = std::function<auth::PasswordValidationContext()>;
    std::unique_ptr<Wt::WValidator> createPasswordStrengthValidator(const auth::IPasswordService& passwordService, PasswordValidationContextGetFunc passwordValidationContextGetFunc);

    // Check current user password
    std::unique_ptr<Wt::WValidator> createPasswordCheckValidator(auth::IPasswordService& passwordService);
} // namespace lms::ui
