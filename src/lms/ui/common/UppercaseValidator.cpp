
#include "UppercaseValidator.hpp"

#include <algorithm>
#include <string>
#include <string_view>

namespace lms::ui
{
    class UppercaseValidator : public Wt::WValidator
    {
    private:
        Wt::WValidator::Result validate(const Wt::WString& input) const override;
        std::string javaScriptValidate() const override { return {}; }
    };

    Wt::WValidator::Result UppercaseValidator::validate(const Wt::WString& input) const
    {
        if (input.empty())
            return Wt::WValidator::validate(input);

        const std::string str{ input.toUTF8() };
        const bool valid{ std::all_of(std::cbegin(str), std::cend(str), [&](char c) { return !std::isalpha(c) || std::isupper(c); }) };

        if (!valid)
            return Wt::WValidator::Result(Wt::ValidationState::Invalid, Wt::WString::tr("Lms.field-must-be-in-upper-case"));

        return Wt::WValidator::Result(Wt::ValidationState::Valid);
    }

    std::unique_ptr<Wt::WValidator> createUppercaseValidator()
    {
        return std::make_unique<UppercaseValidator>();
    }
} // namespace lms::ui
