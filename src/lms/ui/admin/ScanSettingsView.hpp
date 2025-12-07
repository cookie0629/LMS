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
    // ScanSettingsView: 扫描设置视图，允许管理员配置媒体库扫描的参数。
    // ScanSettingsView: представление настроек сканирования, позволяет администратору настраивать параметры сканирования медиатеки.
    //
    // 功能：
    // - 配置扫描间隔（自动扫描的频率）
    // - 配置扫描行为（是否扫描子目录、文件类型过滤等）
    // - 配置元数据提取选项
    // - 配置封面图片提取选项
    // - 保存和恢复扫描设置
    //
    // Функции:
    // - Настраивает интервал сканирования (частота автоматического сканирования)
    // - Настраивает поведение сканирования (сканирование подкаталогов, фильтрация типов файлов и т.д.)
    // - Настраивает опции извлечения метаданных
    // - Настраивает опции извлечения изображений обложек
    // - Сохраняет и восстанавливает настройки сканирования
    class ScanSettingsView : public Wt::WContainerWidget
    {
    public:
        ScanSettingsView();

    private:
        // refreshView: 刷新扫描设置视图，重新加载当前配置。
        // refreshView: обновляет представление настроек сканирования, перезагружает текущую конфигурацию.
        void refreshView();
    };
} // namespace lms::ui
