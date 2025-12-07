/*
 * Copyright (C) 2019 Emeric Poupon
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

#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WTemplate.h>

#include "services/scanner/IScannerService.hpp"

namespace lms::ui
{
    // ScannerController: 扫描器控制器视图，允许管理员监控和控制媒体库扫描过程。
    // ScannerController: представление контроллера сканера, позволяет администратору отслеживать и управлять процессом сканирования медиатеки.
    //
    // 功能：
    // - 显示扫描器当前状态（空闲、扫描中、暂停等）
    // - 显示上次扫描的状态和统计信息
    // - 显示当前扫描步骤的详细信息
    // - 提供扫描报告下载功能
    // - 实时更新扫描进度
    //
    // Функции:
    // - Отображает текущее состояние сканера (простой, сканирование, приостановлено и т.д.)
    // - Отображает статус и статистику последнего сканирования
    // - Отображает подробную информацию о текущем шаге сканирования
    // - Предоставляет функцию загрузки отчёта о сканировании
    // - Обновляет прогресс сканирования в реальном времени
    class ScannerController : public Wt::WTemplate
    {
    public:
        ScannerController();

    private:
        // refreshContents: 刷新整个视图内容。
        // refreshContents: обновляет всё содержимое представления.
        void refreshContents();
        
        // refreshLastScanStatus: 刷新上次扫描的状态显示。
        // refreshLastScanStatus: обновляет отображение статуса последнего сканирования.
        void refreshLastScanStatus(const scanner::IScannerService::Status& status);
        
        // refreshStatus: 刷新当前扫描状态显示。
        // refreshStatus: обновляет отображение текущего статуса сканирования.
        void refreshStatus(const scanner::IScannerService::Status& status);
        
        // refreshCurrentStep: 刷新当前扫描步骤的详细信息。
        // refreshCurrentStep: обновляет подробную информацию о текущем шаге сканирования.
        void refreshCurrentStep(const scanner::ScanStepStats& stepStats);

        Wt::WPushButton* _reportBtn;                      // 报告下载按钮 / кнопка загрузки отчёта
        Wt::WLineEdit* _lastScanStatus;                  // 上次扫描状态显示 / отображение статуса последнего сканирования
        Wt::WLineEdit* _status;                          // 当前状态显示 / отображение текущего статуса
        Wt::WLineEdit* _stepStatus;                      // 当前步骤状态显示 / отображение статуса текущего шага
        class ScannerReportResource* _reportResource;     // 扫描报告资源 / ресурс отчёта о сканировании
    };
} // namespace lms::ui
