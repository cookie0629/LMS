
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
