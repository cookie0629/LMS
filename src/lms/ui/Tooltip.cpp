
#include "Tooltip.hpp"

#include "core/ILogger.hpp"
#include <Wt/WWebWidget.h>

namespace lms::ui
{
    void initTooltipsForWidgetTree(Wt::WWebWidget& widget)
    {
        std::ostringstream oss;
        oss << R"({const rootElement = document.getElementById(')" << widget.id() << R"(');)"
            << R"(const tooltipTriggerList = rootElement.querySelectorAll('[data-bs-toggle="tooltip"]');
tooltipTriggerList.forEach(tooltipTriggerEl => {
new bootstrap.Tooltip(tooltipTriggerEl);
});})";
        LMS_LOG(UI, DEBUG, "Running JS '" << oss.str() << "'");

        widget.doJavaScript(oss.str());
    }
} // namespace lms::ui