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

#include "NotificationContainer.hpp"

#include <sstream>

#include <Wt/WTemplate.h>

#include "core/ILogger.hpp"

#include "LmsApplication.hpp"

namespace lms::ui
{
    namespace
    {
        // NotificationWidget: 单条通知项，对应一个 Bootstrap Toast。
        // NotificationWidget: один Toast‑элемент для уведомления.
        // 功能：显示一条通知消息，根据类型设置不同的颜色样式，并在指定时间后自动关闭。
        // Функции: отображает одно уведомление, устанавливает цвет в зависимости от типа и автоматически закрывает через указанное время.
        class NotificationWidget : public Wt::WTemplate
        {
        public:
            NotificationWidget(Notification::Type type, const Wt::WString& message, std::chrono::milliseconds duration);
            Wt::JSignal<> closed{ this, "closed" };  // 当 Toast 关闭时触发的信号 / сигнал, срабатывающий при закрытии Toast
        };
    } // namespace

    // NotificationWidget 构造函数：创建 Toast 通知，设置样式并通过 JavaScript 显示。
    // Конструктор NotificationWidget: создаёт Toast-уведомление, настраивает стиль и отображает через JavaScript.
    NotificationWidget::NotificationWidget(Notification::Type type, const Wt::WString& message, std::chrono::milliseconds duration)
        : Wt::WTemplate{ Wt::WString::tr("Lms.notifications.template.entry") }
    {
        // 根据通知类型设置背景色和文字颜色
        // Устанавливаем цвет фона и текста в зависимости от типа уведомления
        switch (type)
        {
        case Notification::Type::Info:
            bindString("bg-color", "bg-primary");      // 蓝色背景 / синий фон
            bindString("text-color", "white");
            break;
        case Notification::Type::Warning:
            bindString("bg-color", "bg-warning");        // 黄色背景 / жёлтый фон
            bindString("text-color", "dark");
            break;
        case Notification::Type::Danger:
            bindString("bg-color", "bg-danger");          // 红色背景 / красный фон
            bindString("text-color", "white");
            break;
        }

        bindString("message", message);
        bindInt("duration", duration.count());

        // 生成 JavaScript 代码来显示 Bootstrap Toast
        // Генерируем JavaScript-код для отображения Bootstrap Toast
        std::ostringstream oss;

        oss
            << R"({const toastElement = )" << jsRef() << R"(.getElementsByClassName('toast')[0];)"
            << R"(const toast = bootstrap.Toast.getOrCreateInstance(toastElement);)"
            << R"(toast.show();)"
            << R"(toastElement.addEventListener('hidden.bs.toast', function () {)"
            << closed.createCall({})
            << R"(toast.dispose();)"
            << R"(});})";

        LMS_LOG(UI, DEBUG, "Running JS '" << oss.str() << "'");

        doJavaScript(oss.str());
    }

    // add: 添加一条新通知到容器中，当通知关闭时自动移除 widget。
    // add: добавляет новое уведомление в контейнер, автоматически удаляет widget при закрытии уведомления.
    void NotificationContainer::add(Notification::Type type, const Wt::WString& message, std::chrono::milliseconds duration)
    {
        NotificationWidget* notification{ addNew<NotificationWidget>(type, message, duration) };

        // 当 Toast 关闭时，从容器中移除对应的 widget
        // При закрытии Toast удаляем соответствующий widget из контейнера
        notification->closed.connect([this, notification] {
            removeWidget(notification);
        });
    }
} // namespace lms::ui
