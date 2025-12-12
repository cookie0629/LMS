
#include "DebugToolsView.hpp"

#include "admin/debug/Database.hpp"
#include "debug/Database.hpp"
#include "debug/Tracing.hpp"

namespace lms::ui
{
    DebugToolsView::DebugToolsView()
        : Wt::WTemplate{ Wt::WString::tr("Lms.Admin.DebugTools.template") }
    {
        addFunction("tr", &Wt::WTemplate::Functions::tr);

        bindNew<Tracing>("tracing");
        bindNew<Database>("db");
    }
} // namespace lms::ui
