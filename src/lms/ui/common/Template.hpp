
#pragma once

#include <Wt/WTemplate.h>

namespace lms::ui
{
    // Template: Wt::WTemplate 的轻量包装，重写 applyArguments 以支持自定义参数处理。
    // Template: тонкая обёртка над Wt::WTemplate с переопределением applyArguments для особой обработки аргументов.
    //
    // 功能：
    // - 扩展 Wt::WTemplate 的功能
    // - 提供自定义的参数应用逻辑
    // - 用于所有基于模板的 UI 组件
    //
    // Функции:
    // - Расширяет функциональность Wt::WTemplate
    // - Предоставляет пользовательскую логику применения аргументов
    // - Используется всеми компонентами UI на основе шаблонов
    class Template : public Wt::WTemplate
    {
    public:
        using Wt::WTemplate::WTemplate;

    private:
        // applyArguments: 应用模板参数到 widget（重写以支持自定义处理）。
        // applyArguments: применяет аргументы шаблона к виджету (переопределено для поддержки пользовательской обработки).
        void applyArguments(Wt::WWidget* w, const std::vector<Wt::WString>& args) override;
    };
} // namespace lms::ui
