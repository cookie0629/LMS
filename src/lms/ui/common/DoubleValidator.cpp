
#include "DoubleValidator.hpp"

#include <Wt/WDoubleValidator.h>

namespace lms::ui
{
    class DoubleValidator : public Wt::WDoubleValidator
    {
    public:
        using Wt::WDoubleValidator::WDoubleValidator;

    private:
        std::string javaScriptValidate() const override { return {}; }
    };

    std::unique_ptr<Wt::WValidator> createDoubleValidator(double min, double max)
    {
        auto validator{ std::make_unique<DoubleValidator>(min, max) };
        return validator;
    }
} // namespace lms::ui
