
#include "LoadingIndicator.hpp"

namespace lms::ui
{
    std::unique_ptr<Wt::WTemplate> createLoadingIndicator()
    {
        auto res{ std::make_unique<Wt::WTemplate>(Wt::WString::tr("Lms.LoadingIndicator.template")) };

        res->addFunction("tr", &Wt::WTemplate::Functions::tr);
        res->setScrollVisibilityEnabled(true);
        res->setScrollVisibilityMargin(200);

        return res;
    }
} // namespace lms::ui
