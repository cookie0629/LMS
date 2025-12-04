/*
 * Copyright (C) 2023 Emeric Poupon
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

#include "core/LiteralString.hpp"
#include "services/scanner/ScannerStats.hpp"

namespace lms::scanner
{
    struct ScanContext;

    // IScanStep: 扫描流程中的一个步骤（如扫描文件、检查缺失文件等）的抽象接口。
    // IScanStep: абстракция одного шага сканирования (скан файлов, проверка удалённых файлов и т.п.).
    class IScanStep
    {
    public:
        virtual ~IScanStep() = default;

        virtual ScanStep getStep() const = 0;
        virtual core::LiteralString getStepName() const = 0;
        virtual bool needProcess(const ScanContext& context) const = 0;
        virtual void process(ScanContext& context) = 0;
    };
} // namespace lms::scanner
