
#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WSignal.h>
#include <Wt/WTemplate.h>

#include "database/objects/Filters.hpp"

namespace lms::ui
{
    // Filters: 过滤器组件，允许用户通过聚类、标签、媒体库和专辑类型过滤音乐内容。
    // Filters: компонент фильтров, позволяет пользователям фильтровать музыкальный контент по кластерам, меткам, медиатекам и типам альбомов.
    //
    // 功能：
    // - 显示和管理活动过滤器
    // - 支持聚类过滤器（流派、年份、国家等）
    // - 支持标签过滤器
    // - 支持媒体库过滤器
    // - 支持专辑类型过滤器
    // - 当过滤器改变时发出信号，通知其他视图更新
    //
    // Функции:
    // - Отображает и управляет активными фильтрами
    // - Поддерживает фильтры кластеров (жанр, год, страна и т.д.)
    // - Поддерживает фильтры меток
    // - Поддерживает фильтры медиатек
    // - Поддерживает фильтры типов альбомов
    // - Испускает сигнал при изменении фильтров для обновления других представлений
    class Filters : public Wt::WTemplate
    {
    public:
        Filters();

        // getDbFilters: 获取当前活动的数据库过滤器对象。
        // getDbFilters: получает объект фильтров БД с текущими активными фильтрами.
        const db::Filters& getDbFilters() const { return _dbFilters; }

        // add: 添加聚类过滤器。
        // add: добавляет фильтр кластера.
        void add(db::ClusterId clusterId);

        // updated: 过滤器更新信号，当过滤器改变时发出。
        // updated: сигнал обновления фильтров, испускается при изменении фильтров.
        Wt::Signal<>& updated() { return _sigUpdated; }

    private:
        // showDialog: 显示过滤器设置对话框。
        // showDialog: показывает диалог настройки фильтров.
        void showDialog();
        
        // set: 设置标签过滤器。
        // set: устанавливает фильтр метки.
        void set(db::LabelId labelId);
        
        // set: 设置媒体库过滤器。
        // set: устанавливает фильтр медиатеки.
        void set(db::MediaLibraryId mediaLibraryId);
        
        // set: 设置专辑类型过滤器。
        // set: устанавливает фильтр типа альбома.
        void set(db::ReleaseTypeId releaseTypeId);
        
        // emitFilterAddedNotification: 发出过滤器添加通知。
        // emitFilterAddedNotification: испускает уведомление о добавлении фильтра.
        void emitFilterAddedNotification();

        Wt::WContainerWidget* _filters{};                      // 过滤器容器 / контейнер фильтров
        Wt::WInteractWidget* _mediaLibraryFilter{};            // 媒体库过滤器 widget / виджет фильтра медиатеки
        Wt::WInteractWidget* _labelFilter{};                  // 标签过滤器 widget / виджет фильтра метки
        Wt::WInteractWidget* _releaseTypeFilter{};             // 专辑类型过滤器 widget / виджет фильтра типа альбома

        Wt::Signal<> _sigUpdated;                              // 更新信号 / сигнал обновления

        db::Filters _dbFilters;                                // 数据库过滤器对象 / объект фильтров БД
    };
} // namespace lms::ui
