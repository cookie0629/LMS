
#include "Template.hpp"
#include "core/String.hpp"

namespace lms::ui
{
    void Template::applyArguments(WWidget* widget, const std::vector<Wt::WString>& args)
    {
        for (const Wt::WString& argStr : args)
        {
            std::string arg{ argStr.toUTF8() };
            const std::vector<std::string_view> operands{ core::stringUtils::splitString(arg, '=') };

            if (operands.size() == 2)
            {
                if (operands[0] == "class")
                    widget->addStyleClass(std::string{ operands[1] });
                else
                    widget->setAttributeValue(std::string{ operands[0] }, std::string{ operands[1] });
            }
        }
    }
} // namespace lms::ui
