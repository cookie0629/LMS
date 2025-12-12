
#pragma once

#include <memory>
#include <optional>
#include <utility>

#include <Wt/WContainerWidget.h>
#include <Wt/WSignal.h>
#include <Wt/WString.h>
#include <Wt/WTemplate.h>

namespace lms::ui
{
    // Atomatically raises onRequestElements signal when the sentinel is displayed
    // can add elements afterwards by calling setHasMoreElements()
    // InfiniteScrollingContainer: 无限滚动容器，当用户滚动到底部时自动加载更多内容。
    // InfiniteScrollingContainer: контейнер с бесконечной прокруткой, автоматически загружает больше контента при прокрутке до конца.
    //
    // 功能：
    // - 实现无限滚动（懒加载）模式
    // - 当哨兵元素（sentinel）显示时自动触发 onRequestElements 信号
    // - 显示加载指示器，提示用户正在加载
    // - 支持动态添加和删除元素
    // - 可以通过 setHasMore() 控制是否还有更多内容
    //
    // Функции:
    // - Реализует режим бесконечной прокрутки (ленивая загрузка)
    // - Автоматически вызывает сигнал onRequestElements при отображении элемента-стражника (sentinel)
    // - Отображает индикатор загрузки для уведомления пользователя
    // - Поддерживает динамическое добавление и удаление элементов
    // - Можно управлять наличием дополнительного контента через setHasMore()
    class InfiniteScrollingContainer final : public Wt::WTemplate
    {
    public:
        // "text" must contain loading-indicator and "elements"
        // 模板文本必须包含 "loading-indicator" 和 "elements" 占位符
        // Текст шаблона должен содержать плейсхолдеры "loading-indicator" и "elements"
        InfiniteScrollingContainer(const Wt::WString& text = Wt::WString::tr("Lms.infinite-scrolling-container.template"));

        // reset: 重置容器，清空所有元素。
        // reset: сбрасывает контейнер, очищает все элементы.
        void reset();
        
        // getCount: 获取当前元素数量。
        // getCount: получает текущее количество элементов.
        std::size_t getCount();
        
        // add: 添加一个 widget 到容器。
        // add: добавляет виджет в контейнер.
        void add(std::unique_ptr<Wt::WWidget> result);

        // addNew: 创建并添加新 widget（模板方法）。
        // addNew: создаёт и добавляет новый виджет (шаблонный метод).
        template<typename T, typename... Args>
        T* addNew(Args&&... args)
        {
            return _elements->addNew<T>(std::forward<Args>(args)...);
        }

        // remove: 移除指定的 widget。
        // remove: удаляет указанный виджет.
        void remove(Wt::WWidget& widget);
        
        // remove: 移除指定范围内的 widget。
        // remove: удаляет виджеты в указанном диапазоне.
        void remove(std::size_t first, std::size_t last);

        // getWidget: 获取指定位置的 widget。
        // getWidget: получает виджет по указанной позиции.
        Wt::WWidget* getWidget(std::size_t pos) const;
        
        // getIndexOf: 获取指定 widget 的索引位置。
        // getIndexOf: получает индекс указанного виджета.
        std::optional<std::size_t> getIndexOf(Wt::WWidget& widget) const;
        
        // setHasMore: 设置是否还有更多元素（无参数版本，设置为 true）。
        // setHasMore: устанавливает, есть ли ещё элементы (версия без параметров, устанавливает true).
        void setHasMore(); // can be used to add elements afterwards

        // onRequestElements: 当需要加载更多元素时发出的信号。
        // onRequestElements: сигнал, испускаемый при необходимости загрузки больше элементов.
        Wt::Signal<> onRequestElements;

        // setHasMore: 设置是否还有更多元素（带参数版本）。
        // setHasMore: устанавливает, есть ли ещё элементы (версия с параметром).
        void setHasMore(bool hasMore); // can be used to add elements afterwards

    private:
        void clear() override;
        
        // displayLoadingIndicator: 显示加载指示器。
        // displayLoadingIndicator: отображает индикатор загрузки.
        void displayLoadingIndicator();
        
        // hideLoadingIndicator: 隐藏加载指示器。
        // hideLoadingIndicator: скрывает индикатор загрузки.
        void hideLoadingIndicator();

        Wt::WContainerWidget* _elements;        // 元素容器 / контейнер элементов
        Wt::WTemplate* _loadingIndicator;      // 加载指示器 / индикатор загрузки
    };
} // namespace lms::ui
