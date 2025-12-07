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

#include <Wt/WTemplate.h>

namespace lms::ui
{
    // Template: Wt::WTemplate 的轻量包装，重写 applyArguments 以支持自定义参数处理。
    // Template: тонкая обёртка над Wt::WTemplate с переопределением applyArguments для особой обработки аргументов.
    //
    // 功能：
    // - 扩展 Wt::WTemplate 的功能
    // - 提供自定义的参数应用逻辑
    // - 用于所有基于模板的 UI 组件
    //
    // Функции:
    // - Расширяет функциональность Wt::WTemplate
    // - Предоставляет пользовательскую логику применения аргументов
    // - Используется всеми компонентами UI на основе шаблонов
    class Template : public Wt::WTemplate
    {
    public:
        using Wt::WTemplate::WTemplate;

    private:
        // applyArguments: 应用模板参数到 widget（重写以支持自定义处理）。
        // applyArguments: применяет аргументы шаблона к виджету (переопределено для поддержки пользовательской обработки).
        void applyArguments(Wt::WWidget* w, const std::vector<Wt::WString>& args) override;
    };
} // namespace lms::ui
