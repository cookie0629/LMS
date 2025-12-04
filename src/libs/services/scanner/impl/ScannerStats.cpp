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

#include "services/scanner/ScannerStats.hpp"

namespace lms::scanner
{
    std::size_t ScanStats::getTotalFileCount() const
    {
        return skips + additions + updates + failures;
    }

    std::size_t ScanStats::getChangesCount() const
    {
        return additions + deletions + updates;
    }

    unsigned ScanStepStats::progress() const
    {
        const unsigned res{ static_cast<unsigned>((processedElems / static_cast<float>(totalElems ? totalElems : 1)) * 100) };
        // 理论上可能略高于 100%，因为遍历文件系统时总数可能在增长。
        // Теоретически может немного превышать 100%, так как во время обхода ФС может добавляться больше файлов.
        return res;
    }
} // namespace lms::scanner
