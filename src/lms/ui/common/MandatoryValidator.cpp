
#include "MandatoryValidator.hpp"

namespace lms::ui
{
    class MandatoryValidator : public Wt::WValidator
    {
    private:
        std::string javaScriptValidate() const override { return {}; }
    };

    std::unique_ptr<Wt::WValidator>
    createMandatoryValidator()
    {
        auto v{ std::make_unique<MandatoryValidator>() };
        v->setMandatory(true);
        return v;
    }
} // namespace lms::ui
