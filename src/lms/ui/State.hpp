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

#include <optional>
#include <string>
#include <string_view>

#include "core/String.hpp"

// state: 客户端状态管理命名空间，用于在浏览器会话中存储和读取用户偏好设置。
// state: пространство имён управления состоянием клиента, используется для хранения и чтения пользовательских настроек в сессии браузера.
//
// 功能：
// - 在浏览器会话中持久化用户设置（如播放器设置、UI 状态等）
// - 使用类型安全的模板函数读写各种类型的值
// - 支持枚举类型、数值类型和字符串类型
// - 使用 Wt 的客户端存储机制（localStorage 或 sessionStorage）
//
// Функции:
// - Сохраняет пользовательские настройки в сессии браузера (например, настройки плеера, состояние UI и т.д.)
// - Использует типобезопасные шаблонные функции для чтения/записи значений различных типов
// - Поддерживает типы перечислений, числовые типы и строки
// - Использует механизм хранения клиента Wt (localStorage или sessionStorage)
namespace lms::ui::state
{
    namespace details
    {
        // readValue: 从客户端存储中读取字符串值（底层实现）。
        // readValue: читает строковое значение из хранилища клиента (низкоуровневая реализация).
        std::optional<std::string> readValue(std::string_view item);
        
        // writeValue: 将字符串值写入客户端存储（底层实现）。
        // writeValue: записывает строковое значение в хранилище клиента (низкоуровневая реализация).
        void writeValue(std::string_view item, std::string_view value);
        
        // eraseValue: 从客户端存储中删除指定项（底层实现）。
        // eraseValue: удаляет указанный элемент из хранилища клиента (низкоуровневая реализация).
        void eraseValue(std::string_view item);
    } // namespace details

    // writeValue: 将任意类型的值写入客户端存储（模板函数）。
    // writeValue: записывает значение любого типа в хранилище клиента (шаблонная функция).
    // 支持枚举类型、数值类型和字符串类型。如果值为空，则删除该项。
    // Поддерживает типы перечислений, числовые типы и строки. Если значение пустое, удаляет элемент.
    template<typename T>
    void writeValue(std::string_view item, std::optional<T> value)
    {
        if (value.has_value())
        {
            if constexpr (std::is_enum_v<T>)
                details::writeValue(item, std::to_string(static_cast<std::underlying_type_t<T>>(*value)));
            else
                details::writeValue(item, std::to_string(*value));
        }
        else
            details::eraseValue(item);
    }

    // readValue: 从客户端存储中读取任意类型的值（模板函数）。
    // readValue: читает значение любого типа из хранилища клиента (шаблонная функция).
    // 如果值不存在或无法转换，返回 std::nullopt。
    // Возвращает std::nullopt, если значение отсутствует или не может быть преобразовано.
    template<typename T>
    std::optional<T> readValue(std::string_view item)
    {
        if (std::optional<std::string> res{ details::readValue(item) })
            return core::stringUtils::readAs<T>(*res);

        return std::nullopt;
    }

} // namespace lms::ui::state
