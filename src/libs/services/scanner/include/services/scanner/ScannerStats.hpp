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

#include <Wt/WDateTime.h>

#include <memory>
#include <vector>

#include "database/objects/TrackId.hpp"

#include "ScanErrors.hpp"

namespace lms::scanner
{
    enum class DuplicateReason
    {
        SameHash,
        SameTrackMBID,
    };

    // ScanDuplicate: 标记数据库中被视为“重复”的曲目及原因。
    // ScanDuplicate: описывает трек, признанный дубликатом, и причину (по хэшу или MusicBrainz ID).
    struct ScanDuplicate
    {
        db::TrackId trackId;
        DuplicateReason reason;
    };

    enum class ScanStep
    {
        AssociateArtistImages,
        AssociateExternalLyrics,
        AssociatePlayListTracks,
        AssociateReleaseImages,
        AssociateTrackImages,
        CheckForDuplicatedFiles,
        CheckForRemovedFiles,
        ComputeClusterStats,
        Compact,
        FetchTrackFeatures,
        Optimize,
        ReconciliateArtists,
        ReloadSimilarityEngine,
        RemoveOrphanedDbEntries,
        ScanFiles,
        UpdateLibraryFields,
    };

    // ScanStepStats: 单个扫描步骤的实时进度信息（当前步骤、第几个、总数等）。
    // ScanStepStats: статистика по одному шагу сканирования (текущий шаг, индекс, прогресс).
    struct ScanStepStats
    {
        Wt::WDateTime startTime;

        std::size_t stepCount{};
        std::size_t stepIndex{};
        ScanStep currentStep;

        std::size_t totalElems{};
        std::size_t processedElems{};

        unsigned progress() const;
    };

    // ScanStats: 一次完整扫描的统计数据（文件数、增删改、错误、重复等）。
    // ScanStats: полная статистика по запуску сканера (кол-во файлов, добавления/удаления/ошибки/дубликаты).
    struct ScanStats
    {
        Wt::WDateTime startTime;
        Wt::WDateTime stopTime;

        std::size_t totalFileCount{};

        std::size_t skips{}; // 自上次扫描以来无变化的文件数 / файлов без изменений
        std::size_t scans{}; // 实际被扫描的文件数 / реально просканированных файлов

        std::size_t additions{}; // 新增入库 / добавлено в БД
        std::size_t deletions{}; // 从库中删除 / удалено из БД
        std::size_t updates{};   // 元数据更新 / обновлено
        std::size_t failures{};  // 扫描失败次数 / неудачные сканы

        std::size_t featuresFetched{}; // 获取/更新的特征数量 / количество загруженных фич

        static constexpr std::size_t maxStoredErrorCount{ 5'000 }; // 最大保存在内存中的错误条目数 / максимум ошибок, хранимых в памяти
        std::vector<std::shared_ptr<ScanError>> errors;
        std::size_t errorsCount{}; // maybe bigger than errors.size() if too many errors
        std::vector<ScanDuplicate> duplicates;

        std::size_t getTotalFileCount() const;
        std::size_t getChangesCount() const;
    };
} // namespace lms::scanner
