
#pragma once

#include <optional>

#include <Wt/WComboBox.h>
#include <Wt/WTemplate.h>

#include "database/Types.hpp"

#include "ArtistCollector.hpp"
#include "common/Template.hpp"

namespace lms::ui
{
    class Filters;
    class InfiniteScrollingContainer;

    // Artists: 艺术家列表视图，显示所有艺术家并支持过滤和搜索。
    // Artists: представление списка исполнителей, отображает всех исполнителей и поддерживает фильтрацию и поиск.
    //
    // 功能：
    // - 显示艺术家列表（支持无限滚动）
    // - 支持按艺术家类型过滤（主艺术家、客串艺术家等）
    // - 支持搜索功能
    // - 支持多种排序模式（随机、按名称、按最近添加等）
    // - 使用 ArtistCollector 从数据库收集艺术家数据
    //
    // Функции:
    // - Отображает список исполнителей (с поддержкой бесконечной прокрутки)
    // - Поддерживает фильтрацию по типу исполнителя (основной, приглашённый и т.д.)
    // - Поддерживает функцию поиска
    // - Поддерживает различные режимы сортировки (случайный, по имени, по недавно добавленным и т.д.)
    // - Использует ArtistCollector для сбора данных об исполнителях из БД
    class Artists : public Template
    {
    public:
        Artists(Filters& filters);

    private:
        // refreshView: 刷新视图（无参数版本，使用默认设置）。
        // refreshView: обновляет представление (версия без параметров, использует настройки по умолчанию).
        void refreshView();
        
        // refreshView: 根据排序模式刷新视图。
        // refreshView: обновляет представление в соответствии с режимом сортировки.
        void refreshView(ArtistCollector::Mode mode);
        
        // refreshView: 根据艺术家链接类型刷新视图。
        // refreshView: обновляет представление в соответствии с типом связи исполнителя.
        void refreshView(std::optional<db::TrackArtistLinkType> linkType);
        
        // refreshView: 根据搜索文本刷新视图。
        // refreshView: обновляет представление в соответствии с текстом поиска.
        void refreshView(const Wt::WString& searchText);
        
        // addSome: 添加一批艺术家到视图（无限滚动时调用）。
        // addSome: добавляет группу исполнителей в представление (вызывается при бесконечной прокрутке).
        void addSome();

        static constexpr std::size_t _batchSize{ 30 };      // 每批加载的艺术家数量 / количество исполнителей за загрузку
        static constexpr std::size_t _maxCount{ 8000 };      // 最大显示数量 / максимальное количество для отображения

        Wt::WWidget* _currentLinkTypeActiveItem{};         // 当前活动的链接类型项 / текущий активный элемент типа связи
        InfiniteScrollingContainer* _container{};          // 无限滚动容器 / контейнер с бесконечной прокруткой
        ArtistCollector _artistCollector;                  // 艺术家收集器 / сборщик исполнителей
        static constexpr ArtistCollector::Mode _defaultSortMode{ ArtistCollector::Mode::Random };  // 默认排序模式 / режим сортировки по умолчанию
    };
} // namespace lms::ui
