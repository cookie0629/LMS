
#pragma once

#include "ReleaseCollector.hpp"
#include "common/Template.hpp"

namespace lms::ui
{
    class Filters;
    class InfiniteScrollingContainer;
    class PlayQueueController;

    // Releases: 专辑列表视图，以网格形式显示专辑封面，支持过滤、搜索和播放操作。
    // Releases: представление списка альбомов, отображает обложки альбомов в виде сетки, поддерживает фильтрацию, поиск и операции воспроизведения.
    //
    // 功能：
    // - 以网格形式显示专辑（每行 6 个）
    // - 支持无限滚动加载更多专辑
    // - 支持搜索功能
    // - 支持多种排序模式（随机、按名称、按日期等）
    // - 支持将专辑添加到播放队列
    // - 使用 ReleaseCollector 从数据库收集专辑数据
    //
    // Функции:
    // - Отображает альбомы в виде сетки (6 на строку)
    // - Поддерживает бесконечную прокрутку для загрузки больше альбомов
    // - Поддерживает функцию поиска
    // - Поддерживает различные режимы сортировки (случайный, по имени, по дате и т.д.)
    // - Поддерживает добавление альбомов в очередь воспроизведения
    // - Использует ReleaseCollector для сбора данных об альбомах из БД
    class Releases : public Template
    {
    public:
        Releases(Filters& filters, PlayQueueController& playQueueController);

    private:
        // refreshView: 刷新视图（无参数版本，使用默认设置）。
        // refreshView: обновляет представление (версия без параметров, использует настройки по умолчанию).
        void refreshView();
        
        // refreshView: 根据排序模式刷新视图。
        // refreshView: обновляет представление в соответствии с режимом сортировки.
        void refreshView(ReleaseCollector::Mode mode);
        
        // refreshView: 根据搜索文本刷新视图。
        // refreshView: обновляет представление в соответствии с текстом поиска.
        void refreshView(const Wt::WString& searchText);

        // addSome: 添加一批专辑到视图（无限滚动时调用）。
        // addSome: добавляет группу альбомов в представление (вызывается при бесконечной прокрутке).
        void addSome();
        
        // getAllReleases: 获取所有符合条件的专辑 ID（用于播放操作）。
        // getAllReleases: получает ID всех подходящих альбомов (для операций воспроизведения).
        std::vector<db::ReleaseId> getAllReleases();

        static constexpr std::size_t _maxItemsPerLine{ 6 };      // 每行最大项目数 / максимальное количество элементов на строку
        static constexpr std::size_t _batchSize{ _maxItemsPerLine };  // 每批加载数量 / количество за загрузку
        static constexpr std::size_t _maxCount{ _maxItemsPerLine * 500 };  // 最大显示数量（3000 个专辑）/ максимальное количество для отображения (3000 альбомов)

        PlayQueueController& _playQueueController;              // 播放队列控制器 / контроллер очереди воспроизведения
        InfiniteScrollingContainer* _container{};              // 无限滚动容器 / контейнер с бесконечной прокруткой
        ReleaseCollector _releaseCollector;                    // 专辑收集器 / сборщик альбомов
        static constexpr ReleaseCollector::Mode _defaultMode{ ReleaseCollector::Mode::Random };  // 默认排序模式 / режим сортировки по умолчанию
    };
} // namespace lms::ui
