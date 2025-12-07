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

#include <Wt/WTemplate.h>

#include "PlayQueueController.hpp"

namespace lms::ui
{
    class Filters;
    class SearchView;
    class PlayQueue;

    // Explore: 音乐探索视图的主容器，管理艺术家、专辑、曲目等视图的切换。
    // Explore: главный контейнер представления исследования музыки, управляет переключением между представлениями исполнителей, альбомов, треков и т.д.
    //
    // 功能：
    // - 作为音乐库浏览的主界面容器
    // - 管理播放队列控制器，处理从探索视图到播放队列的操作
    // - 协调过滤器、搜索和各个子视图（艺术家、专辑、曲目等）
    //
    // Функции:
    // - Служит главным контейнером интерфейса просмотра музыкальной библиотеки
    // - Управляет контроллером очереди воспроизведения, обрабатывает операции от представления исследования к очереди воспроизведения
    // - Координирует фильтры, поиск и различные подпредставления (исполнители, альбомы, треки и т.д.)
    class Explore : public Wt::WTemplate
    {
    public:
        Explore(Filters& filters, PlayQueue& playQueue);

        // getPlayQueueController: 获取播放队列控制器，用于从探索视图操作播放队列。
        // getPlayQueueController: получает контроллер очереди воспроизведения для управления очередью из представления исследования.
        PlayQueueController& getPlayQueueController() { return _playQueueController; }

    private:
        PlayQueueController _playQueueController;  // 播放队列控制器 / контроллер очереди воспроизведения
    };
} // namespace lms::ui
