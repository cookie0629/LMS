
#include "PasswordValidator.hpp"

#include <Wt/WEnvironment.h>

#include "services/auth/IPasswordService.hpp"

#include "LmsApplication.hpp"

namespace lms::ui
{
    namespace
    {
        class PasswordStrengthValidator : public Wt::WValidator
        {
        public:
            PasswordStrengthValidator(const auth::IPasswordService& passwordService, PasswordValidationContextGetFunc passwordValidationContextGetFunc)
                : _passwordValidationContextGetFunc{ std::move(passwordValidationContextGetFunc) }
                , _passwordService{ passwordService }
            {
            }

            ~PasswordStrengthValidator() override = default;
            PasswordStrengthValidator(const PasswordStrengthValidator&) = delete;
            PasswordStrengthValidator& operator=(const PasswordStrengthValidator&) = delete;

        private:
            Wt::WValidator::Result validate(const Wt::WString& input) const override;
            std::string javaScriptValidate() const override { return {}; }

            PasswordValidationContextGetFunc _passwordValidationContextGetFunc;
            const auth::IPasswordService& _passwordService;
        };
    } // namespace

    Wt::WValidator::Result PasswordStrengthValidator::validate(const Wt::WString& input) const
    {
        if (input.empty())
            return Wt::WValidator::validate(input);

        const auth::PasswordValidationContext context{ _passwordValidationContextGetFunc() };

        switch (_passwordService.checkPasswordAcceptability(input.toUTF8(), context))
        {
        case auth::IPasswordService::PasswordAcceptabilityResult::OK:
            return Wt::WValidator::Result{ Wt::ValidationState::Valid };
        case auth::IPasswordService::PasswordAcceptabilityResult::TooWeak:
            return Wt::WValidator::Result{ Wt::ValidationState::Invalid, Wt::WString::tr("Lms.password-too-weak") };
        case auth::IPasswordService::PasswordAcceptabilityResult::MustMatchLoginName:
            return Wt::WValidator::Result{ Wt::ValidationState::Invalid, Wt::WString::tr("Lms.password-must-match-login") };
        }

        throw core::LmsException{ "internal error" };
    }

    std::unique_ptr<Wt::WValidator> createPasswordStrengthValidator(const auth::IPasswordService& passwordService, PasswordValidationContextGetFunc passwordValidationContextGetFunc)
    {
        return std::make_unique<PasswordStrengthValidator>(passwordService, std::move(passwordValidationContextGetFunc));
    }

    class PasswordCheckValidator : public Wt::WValidator
    {
    public:
        PasswordCheckValidator(auth::IPasswordService& passwordService)
            : _passwordService{ passwordService }
        {
        }
        ~PasswordCheckValidator() = default;
        PasswordCheckValidator(const PasswordCheckValidator&) = delete;
        PasswordCheckValidator& operator=(const PasswordCheckValidator&) = delete;

    private:
        Wt::WValidator::Result validate(const Wt::WString& input) const override;
        std::string javaScriptValidate() const override { return {}; }

        auth::IPasswordService& _passwordService;
    };

    Wt::WValidator::Result PasswordCheckValidator::validate(const Wt::WString& input) const
    {
        if (input.empty())
            return Wt::WValidator::validate(input);

        const auto checkResult{ _passwordService.checkUserPassword(
            boost::asio::ip::make_address(LmsApp->environment().clientAddress()),
            LmsApp->getUserLoginName(),
            input.toUTF8()) };
        switch (checkResult.state)
        {
        case auth::IPasswordService::CheckResult::State::Granted:
            return Wt::WValidator::Result{ Wt::ValidationState::Valid };
        case auth::IPasswordService::CheckResult::State::Denied:
            return Wt::WValidator::Result{ Wt::ValidationState::Invalid, Wt::WString::tr("Lms.Settings.password-bad") };
        case auth::IPasswordService::CheckResult::State::Throttled:
            return Wt::WValidator::Result{ Wt::ValidationState::Invalid, Wt::WString::tr("Lms.password-client-throttled") };
        }

        throw core::LmsException{ "InternalError" };
    }

    std::unique_ptr<Wt::WValidator> createPasswordCheckValidator(auth::IPasswordService& passwordService)
    {
        return std::make_unique<PasswordCheckValidator>(passwordService);
    }

} // namespace lms::ui