/*
 * Copyright (C) 2024 Emeric Poupon
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

#include "database/objects/MediaLibraryId.hpp"

namespace lms::ui
{
    // MediaLibrariesView: 媒体库管理视图，允许管理员管理音乐库的配置。
    // MediaLibrariesView: представление управления медиатеками, позволяет администратору управлять конфигурацией музыкальных библиотек.
    //
    // 功能：
    // - 显示所有已配置的媒体库
    // - 允许添加新的媒体库
    // - 允许编辑现有媒体库的配置（路径、扫描设置等）
    // - 允许删除媒体库（带确认对话框）
    // - 显示每个媒体库的状态和统计信息
    //
    // Функции:
    // - Отображает все настроенные медиатеки
    // - Позволяет добавлять новые медиатеки
    // - Позволяет редактировать конфигурацию существующих медиатек (путь, настройки сканирования и т.д.)
    // - Позволяет удалять медиатеки (с диалогом подтверждения)
    // - Отображает статус и статистику каждой медиатеки
    class MediaLibrariesView : public Wt::WTemplate
    {
    public:
        MediaLibrariesView();

    private:
        // refreshView: 刷新媒体库列表视图。
        // refreshView: обновляет представление списка медиатек.
        void refreshView();
        
        // showDeleteLibraryModal: 显示删除媒体库的确认对话框。
        // showDeleteLibraryModal: показывает диалог подтверждения удаления медиатеки.
        void showDeleteLibraryModal(db::MediaLibraryId library, Wt::WTemplate* libraryEntry);
        
        // updateEntry: 更新指定媒体库的显示条目。
        // updateEntry: обновляет элемент отображения указанной медиатеки.
        void updateEntry(db::MediaLibraryId library, Wt::WTemplate* libraryEntry);
        
        // addEntry: 添加新的媒体库条目到列表。
        // addEntry: добавляет новый элемент медиатеки в список.
        Wt::WTemplate* addEntry();

        Wt::WContainerWidget* _libraries{};  // 媒体库列表容器 / контейнер списка медиатек
    };
} // namespace lms::ui