
#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>

#include "LmsInitApplication.hpp"

namespace lms::ui
{
    LmsInitApplication::LmsInitApplication(const Wt::WEnvironment& env)
        : Wt::WApplication{ env }
    {
        enableUpdates(true);
        root()->addNew<Wt::WText>("LMS is initializing. This may take a while, please wait...");
    }

    std::unique_ptr<Wt::WApplication> LmsInitApplication::create(const Wt::WEnvironment& env)
    {
        return std::make_unique<LmsInitApplication>(env);
    }
} // namespace lms::ui