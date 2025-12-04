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
