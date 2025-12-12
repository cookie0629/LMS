
#pragma once

#include <Wt/WContainerWidget.h>

namespace lms::ui
{

    class UserView : public Wt::WContainerWidget
    {
    public:
        UserView();

    private:
        void refreshView();
    };

} // namespace lms::ui
