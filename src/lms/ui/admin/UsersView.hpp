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

#include <Wt/WContainerWidget.h>
#include <Wt/WTemplate.h>

namespace lms::ui
{
    // UsersView: 管理员用的"用户列表"视图，负责展示/刷新所有用户。
    // UsersView: административное представление списка пользователей, умеет обновлять/перерисовывать список.
    //
    // 功能：
    // - 显示系统中所有用户的列表
    // - 显示用户的基本信息（用户名、类型、最后登录时间等）
    // - 提供创建新用户的入口
    // - 提供编辑和删除用户的功能
    // - 支持刷新用户列表
    //
    // Функции:
    // - Отображает список всех пользователей в системе
    // - Отображает основную информацию о пользователях (имя пользователя, тип, время последнего входа и т.д.)
    // - Предоставляет точку входа для создания нового пользователя
    // - Предоставляет функции редактирования и удаления пользователей
    // - Поддерживает обновление списка пользователей
    class UsersView : public Wt::WTemplate
    {
    public:
        UsersView();

    private:
        // refreshView: 刷新用户列表视图，重新加载所有用户数据。
        // refreshView: обновляет представление списка пользователей, перезагружает все данные пользователей.
        void refreshView();

        Wt::WContainerWidget* _container;  // 用户列表容器 / контейнер списка пользователей
    };

} // namespace lms::ui
