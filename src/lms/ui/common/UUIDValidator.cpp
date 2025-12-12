
#include "UUIDValidator.hpp"

#include <Wt/WRegExpValidator.h>

namespace lms::ui
{
    class RegExpValidator : public Wt::WRegExpValidator
    {
    public:
        using Wt::WRegExpValidator::WRegExpValidator;

    private:
        std::string javaScriptValidate() const override { return {}; }
    };

    std::unique_ptr<Wt::WValidator> createUUIDValidator()
    {
        auto validator{ std::make_unique<RegExpValidator>("[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{12}") };
        validator->setInvalidNoMatchText(Wt::WString::tr("Lms.uuid-invalid"));

        return validator;
    }
} // namespace lms::ui
