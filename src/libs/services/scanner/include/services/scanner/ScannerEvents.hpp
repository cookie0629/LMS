/*
 * Copyright (C) 2020 Emeric Poupon
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

#include <Wt/WDateTime.h>
#include <Wt/WSignal.h>

#include "ScannerStats.hpp"

namespace lms::scanner
{

    // Events: 扫描服务对外暴露的一组信号，用于 UI 显示进度和结果。
    // Events: набор сигналов сканера, который слушают UI и другие подсистемы для отображения прогресса/результатов.
    struct Events
    {
        Wt::Signal<> scanAborted;

        Wt::Signal<> scanStarted;

        Wt::Signal<ScanStats> scanComplete;

        Wt::Signal<ScanStepStats> scanInProgress;

        Wt::Signal<Wt::WDateTime> scanScheduled;
    };

} // namespace lms::scanner
