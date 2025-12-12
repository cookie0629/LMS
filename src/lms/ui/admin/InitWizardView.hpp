
#pragma once

#include <Wt/WTemplateFormView.h>

namespace lms::auth
{
    class IPasswordService;
}

namespace lms::ui
{
    // InitWizardView: 初始化向导视图，在首次启动时引导管理员创建第一个用户。
    // InitWizardView: представление мастера инициализации, направляет администратора при первом запуске для создания первого пользователя.
    //
    // 功能：
    // - 在系统首次启动时显示（当数据库中没有用户时）
    // - 引导管理员创建第一个管理员账户
    // - 设置初始用户名和密码
    // - 完成初始化后自动跳转到主界面
    //
    // Функции:
    // - Отображается при первом запуске системы (когда в БД нет пользователей)
    // - Направляет администратора для создания первой учётной записи администратора
    // - Устанавливает начальное имя пользователя и пароль
    // - После завершения инициализации автоматически переходит на главный интерфейс
    class InitWizardView : public Wt::WTemplateFormView
    {
    public:
        InitWizardView(auth::IPasswordService& passwordService);
    };

} // namespace lms::ui
