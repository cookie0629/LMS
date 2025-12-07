/*
 * Copyright (C) 2018 Emeric Poupon
 *
 * This file is part of LMS.
 *
 * LMS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LMS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LMS.  If not, see <http://www.gnu.org/licenses/>.
 */

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
