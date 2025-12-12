
#pragma once

#include <Wt/WContainerWidget.h>

namespace lms::ui
{
    // SettingsView: 用户设置页面根容器，负责在访问 /settings 时构建并显示设置表单。
    // SettingsView: корневой виджет страницы настроек пользователя, создаёт и показывает форму при переходе на /settings.
    class SettingsView : public Wt::WContainerWidget
    {
    public:
        SettingsView();

    private:
        void refreshView();
    };

} // namespace lms::ui
