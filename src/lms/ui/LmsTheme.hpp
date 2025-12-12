
#pragma once

#include <Wt/WConfig.h>
#include <Wt/WTheme.h>

namespace lms::ui
{
    // LmsTheme: 基于 Bootstrap5 的自定义主题，配置全局 CSS/JS 和校验样式。
    // LmsTheme: настраиваемая тема на базе Bootstrap5, подключает CSS/JS и управляет валидационными классами.
    //
    // 功能：
    // - 集成 Bootstrap 5 CSS 和 JavaScript
    // - 配置表单验证样式（有效/无效状态）
    // - 定义 CSS 工具类（禁用、激活等）
    // - 支持将锚点样式化为按钮
    //
    // Функции:
    // - Интегрирует Bootstrap 5 CSS и JavaScript
    // - Настраивает стили валидации форм (валидное/невалидное состояние)
    // - Определяет CSS-утилиты (отключено, активно и т.д.)
    // - Поддерживает стилизацию якорей как кнопок
    class LmsTheme : public Wt::WTheme
    {
    private:
        // init: 初始化主题，加载必要的 CSS 和 JavaScript 资源。
        // init: инициализирует тему, загружает необходимые ресурсы CSS и JavaScript.
        void init(Wt::WApplication* app) const override;

        // name: 返回主题名称。
        // name: возвращает имя темы.
        std::string name() const override;
        
        // resourcesUrl: 返回主题资源的基础 URL。
        // resourcesUrl: возвращает базовый URL ресурсов темы.
        std::string resourcesUrl() const override;
        
        // styleSheets: 返回主题使用的 CSS 样式表列表。
        // styleSheets: возвращает список CSS-таблиц стилей, используемых темой.
        std::vector<Wt::WLinkedCssStyleSheet> styleSheets() const override;
        
        void apply(Wt::WWidget*, Wt::WWidget*, int) const override {};
        void apply(Wt::WWidget*, Wt::DomElement&, int) const override {};
        
        // disabledClass: 返回禁用状态的 CSS 类名。
        // disabledClass: возвращает имя CSS-класса для отключённого состояния.
        std::string disabledClass() const override { return "disabled"; }
        
        // activeClass: 返回激活状态的 CSS 类名。
        // activeClass: возвращает имя CSS-класса для активного состояния.
        std::string activeClass() const override { return "active"; };
        
        std::string utilityCssClass(int) const override { return ""; };
        
        // canStyleAnchorAsButton: 是否支持将锚点样式化为按钮。
        // canStyleAnchorAsButton: поддерживается ли стилизация якорей как кнопок.
        bool canStyleAnchorAsButton() const override { return true; };
        
        // applyValidationStyle: 应用表单验证样式（有效/无效状态）。
        // applyValidationStyle: применяет стили валидации форм (валидное/невалидное состояние).
        void applyValidationStyle(Wt::WWidget* widget,
                                  const Wt::WValidator::Result& validation,
                                  Wt::WFlags<Wt::ValidationStyleFlag> flags) const override;
        
        bool canBorderBoxElement(const Wt::DomElement&) const override { return true; }

#if WT_VERSION >= 0x040C0100
        void applyFunctionalStyling(Wt::WWidget*, Wt::WWidget*, int) const override {}
        void applyOptionalStyling(Wt::WWidget*, Wt::WWidget*, int) const override {}
#endif
    };
} // namespace lms::ui
