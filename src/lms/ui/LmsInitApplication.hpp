
#pragma once

#include <Wt/WApplication.h>

namespace lms::ui
{
    class LmsInitApplication : public Wt::WApplication
    {
    public:
        LmsInitApplication(const Wt::WEnvironment& env);

        static std::unique_ptr<Wt::WApplication> create(const Wt::WEnvironment& env);
    };
} // namespace lms::ui