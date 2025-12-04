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

namespace lms::scanner
{
    // ScanOptions: 控制一次扫描行为的可选参数。
    // ScanOptions: опции, определяющие поведение одного прохода сканера.
    struct ScanOptions
    {
        bool fullScan{};      // 即使文件元数据未变也重新扫描 / пересканировать файлы даже без изменений
        bool forceOptimize{}; // 强制优化数据库 / принудительно оптимизировать БД
        bool compact{};       // 压缩数据库文件 / сжать файл базы данных
    };
} // namespace lms::scanner