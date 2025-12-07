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

namespace Wt
{
    class WWebWidget;
}

namespace lms::ui
{
    // initTooltipsForWidgetTree: 为 widget 树中的所有元素初始化 Bootstrap Tooltip。
    // initTooltipsForWidgetTree: инициализирует Bootstrap Tooltip для всех элементов дерева виджетов.
    //
    // 功能：
    // - 递归遍历 widget 树
    // - 为所有具有 tooltip 属性的元素初始化 Bootstrap Tooltip
    // - 确保 tooltip 在动态添加的 widget 上也能正常工作
    //
    // Функции:
    // - Рекурсивно обходит дерево виджетов
    // - Инициализирует Bootstrap Tooltip для всех элементов с атрибутом tooltip
    // - Обеспечивает работу tooltip на динамически добавленных виджетах
    void initTooltipsForWidgetTree(Wt::WWebWidget& widget);
}