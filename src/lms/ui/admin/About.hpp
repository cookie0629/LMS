/*
 * Copyright (C) 2025 Emeric Poupon
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

namespace lms::ui
{
    // showAboutModal: 显示"关于"模态框，展示 LMS 的版本信息和系统信息。
    // showAboutModal: показывает модальное окно "О программе", отображает информацию о версии LMS и системную информацию.
    //
    // 功能：
    // - 显示 LMS 版本号
    // - 显示构建信息
    // - 显示系统信息（操作系统、编译器版本等）
    // - 显示依赖库的版本信息
    //
    // Функции:
    // - Отображает номер версии LMS
    // - Отображает информацию о сборке
    // - Отображает системную информацию (операционная система, версия компилятора и т.д.)
    // - Отображает информацию о версиях зависимых библиотек
    void showAboutModal();
} // namespace lms::ui
