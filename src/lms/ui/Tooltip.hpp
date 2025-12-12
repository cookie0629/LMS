
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