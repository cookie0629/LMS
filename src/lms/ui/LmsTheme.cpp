/*
 * Copyright (C) 2020 Emweb bv, Herent, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include "LmsTheme.hpp"

#include <Wt/WApplication.h>
#include <Wt/WLinkedCssStyleSheet.h>

namespace lms::ui
{
    // init: 在应用启动时注入需要的 JS 资源（例如 Bootstrap bundle）。
    // init: подключает необходимые JS‑ресурсы при старте приложения (Bootstrap bundle и т.п.).
    void LmsTheme::init(Wt::WApplication* app) const
    {
        app->require("js/bootstrap.bundle.min.js");
    }

    // name: 返回主题名称，Wt 用它来选择默认样式。
    // name: возвращает имя темы, которое использует Wt для выбора стилей.
    std::string LmsTheme::name() const
    {
        return "bootstrap5";
    }

    // resourcesUrl: 额外静态资源前缀，这里留空表示使用默认路径。
    // resourcesUrl: префикс для статических ресурсов, здесь пустая строка = путь по умолчанию.
    std::string LmsTheme::resourcesUrl() const
    {
        return "";
    }

    // styleSheets: 返回需要加载的 CSS 列表（Bootstrap 皮肤 + LMS 自定义样式）。
    // styleSheets: список CSS‑файлов для подключения (тема Bootstrap и собственный стиль LMS).
    std::vector<Wt::WLinkedCssStyleSheet> LmsTheme::styleSheets() const
    {
        static const std::vector<Wt::WLinkedCssStyleSheet> files{
            { "css/bootstrap.solar.min.css" },
            { "css/lms.css" },
        };

        return files;
    }

    void LmsTheme::applyValidationStyle(Wt::WWidget* widget, const Wt::WValidator::Result& validation, Wt::WFlags<Wt::ValidationStyleFlag> styles) const
    {
        {
            const bool validStyle{ (validation.state() == Wt::ValidationState::Valid) && styles.test(Wt::ValidationStyleFlag::ValidStyle) };
            widget->toggleStyleClass("is-valid", validStyle);
        }

        {
            const bool invalidStyle{ (validation.state() != Wt::ValidationState::Valid) && styles.test(Wt::ValidationStyleFlag::InvalidStyle) };
            widget->toggleStyleClass("is-invalid", invalidStyle);
        }
    }

} // namespace lms::ui
