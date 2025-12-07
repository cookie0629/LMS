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

#include <vector>

#include "database/objects/ArtistId.hpp"
#include "database/objects/MediumId.hpp"
#include "database/objects/ReleaseId.hpp"
#include "database/objects/TrackId.hpp"
#include "database/objects/TrackListId.hpp"

namespace lms::ui
{
    class Filters;
    class PlayQueue;

    // Used to interact with the play queue, using the current exploration filters
    // PlayQueueController: 播放队列控制器，用于从探索视图操作播放队列，考虑当前活动的过滤器。
    // PlayQueueController: контроллер очереди воспроизведения, используется для управления очередью из представления исследования с учётом текущих активных фильтров.
    //
    // 功能：
    // - 将艺术家、专辑、曲目添加到播放队列
    // - 支持多种播放命令（立即播放、下一首播放、追加、随机播放）
    // - 应用当前活动的过滤器（只添加符合过滤条件的曲目）
    // - 限制添加到队列的曲目数量（防止队列过大）
    //
    // Функции:
    // - Добавляет исполнителей, альбомы, треки в очередь воспроизведения
    // - Поддерживает различные команды воспроизведения (немедленное воспроизведение, воспроизведение следующим, добавление, случайное воспроизведение)
    // - Применяет текущие активные фильтры (добавляет только треки, соответствующие фильтрам)
    // - Ограничивает количество треков, добавляемых в очередь (предотвращает переполнение очереди)
    class PlayQueueController
    {
    public:
        PlayQueueController(Filters& filters, PlayQueue& playQueue);

        // Command: 播放命令枚举，定义不同的播放操作。
        // Command: перечисление команд воспроизведения, определяет различные операции воспроизведения.
        enum class Command
        {
            Play,            // 立即播放，清空当前队列 / немедленное воспроизведение, очищает текущую очередь
            PlayNext,        // 下一首播放，插入到当前播放位置之后 / воспроизведение следующим, вставляет после текущей позиции
            PlayOrAddLast,   // 如果队列为空则播放，否则追加到末尾 / воспроизведение, если очередь пуста, иначе добавление в конец
            PlayShuffled,    // 随机播放，清空当前队列 / случайное воспроизведение, очищает текущую очередь
        };

        // processCommand: 处理艺术家列表的播放命令。
        // processCommand: обрабатывает команду воспроизведения для списка исполнителей.
        void processCommand(Command command, const std::vector<db::ArtistId>& artists);
        
        // processCommand: 处理专辑列表的播放命令。
        // processCommand: обрабатывает команду воспроизведения для списка альбомов.
        void processCommand(Command command, const std::vector<db::ReleaseId>& releases);
        
        // processCommand: 处理曲目列表的播放命令。
        // processCommand: обрабатывает команду воспроизведения для списка треков.
        void processCommand(Command command, const std::vector<db::TrackId>& tracks);

        // processCommand: 处理单个介质的播放命令。
        // processCommand: обрабатывает команду воспроизведения для одного носителя.
        void processCommand(Command command, db::MediumId medium);
        
        // processCommand: 处理播放列表的播放命令。
        // processCommand: обрабатывает команду воспроизведения для плейлиста.
        void processCommand(Command command, db::TrackListId trackList);
        
        // playTrackInRelease: 在专辑中播放指定曲目（播放该曲目及其后的所有曲目）。
        // playTrackInRelease: воспроизводит указанный трек в альбоме (воспроизводит этот трек и все последующие).
        void playTrackInRelease(db::TrackId track);

        // setMaxTrackCountToEnqueue: 设置添加到队列的最大曲目数量。
        // setMaxTrackCountToEnqueue: устанавливает максимальное количество треков для добавления в очередь.
        void setMaxTrackCountToEnqueue(std::size_t maxTrackCount) { _maxTrackCountToEnqueue = maxTrackCount; }

    private:
        Filters& _filters;                    // 过滤器引用 / ссылка на фильтры
        PlayQueue& _playQueue;                 // 播放队列引用 / ссылка на очередь воспроизведения
        std::size_t _maxTrackCountToEnqueue{}; // 最大入队曲目数 / максимальное количество треков для добавления в очередь
    };
} // namespace lms::ui
