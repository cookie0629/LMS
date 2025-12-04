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

#include "services/scanner/ScannerOptions.hpp"
#include "services/scanner/ScannerStats.hpp"

namespace lms::scanner
{
    // ScanContext: 一次扫描过程中在各个步骤之间传递的共享上下文（选项 + 全局统计 + 当前步骤统计）。
    // ScanContext: общий контекст одного запуска сканера (опции, глобальная статистика и статистика текущего шага).
    struct ScanContext
    {
        ScanOptions scanOptions;
        ScanStats stats;
        ScanStepStats currentStepStats;
    };
} // namespace lms::scanner