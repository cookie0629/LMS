/*
 * Copyright (C) 2022 Emeric Poupon
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
#include <Wt/WSignal.h>
#include <Wt/WWidget.h>

namespace lms::ui
{
    // ModalManager: 管理页面上的 Bootstrap 模态框（弹窗），负责展示和销毁。
    // ModalManager: менеджер модальных окон Bootstrap на странице, показывает и удаляет модальные виджеты.
    // 
    // 功能：
    // - show(): 显示一个新的模态框，通过 Bootstrap JavaScript API 控制
    // - dispose(): 隐藏并清理模态框
    // - 自动处理模态框关闭事件，从 DOM 中移除 widget
    //
    // Функции:
    // - show(): показывает новое модальное окно, управляется через Bootstrap JavaScript API
    // - dispose(): скрывает и очищает модальное окно
    // - автоматически обрабатывает события закрытия модального окна, удаляет widget из DOM
    class ModalManager : public Wt::WContainerWidget
    {
    public:
        ModalManager();

        // should handle only one modal at a time
        // Widget must contain a "modal" element
        // 注意：一次只应处理一个模态框。Widget 必须包含一个 "modal" 元素。
        // Примечание: должен обрабатывать только одно модальное окно за раз. Виджет должен содержать элемент "modal".
        void show(std::unique_ptr<Wt::WWidget> modalWidget);
        void dispose(Wt::WWidget* modalWidget);

    private:
        // _closed: JavaScript 信号，当模态框在客户端关闭时触发，参数是模态框的 ID。
        // _closed: JavaScript-сигнал, срабатывает при закрытии модального окна на клиенте, параметр — ID модального окна.
        Wt::JSignal<std::string> _closed;
    };
} // namespace lms::ui
