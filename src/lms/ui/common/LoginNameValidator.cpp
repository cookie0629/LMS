
#include "LoginNameValidator.hpp"

#include <Wt/WLengthValidator.h>

#include "database/objects/User.hpp"

namespace lms::ui
{
    namespace
    {
        class LengthValidator : public Wt::WLengthValidator
        {
        private:
            std::string javaScriptValidate() const override { return {}; }
        };
    } // namespace

    std::unique_ptr<Wt::WValidator> createLoginNameValidator()
    {
        auto v = std::make_unique<LengthValidator>();
        v->setMandatory(true);
        v->setMinimumLength(db::User::minNameLength);
        v->setMaximumLength(db::User::maxNameLength);
        return v;
    }
} // namespace lms::ui
