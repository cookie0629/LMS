
#pragma once

#include <chrono>

#include <Wt/WContainerWidget.h>
#include <Wt/WString.h>

#include "Notification.hpp"

namespace lms::ui
{
    // NotificationContainer: 页面右上角的通知区域，负责显示/自动移除 Toast 消息。
    // NotificationContainer: контейнер уведомлений в UI, показывает и автоматически скрывает Toast‑сообщения.
    //
    // 功能：
    // - 管理所有通知消息的显示
    // - 支持三种类型的通知：信息（蓝色）、警告（黄色）、错误（红色）
    // - 通知会在指定时间后自动消失
    // - 使用 Bootstrap Toast 组件实现
    //
    // Функции:
    // - Управляет отображением всех уведомлений
    // - Поддерживает три типа уведомлений: информация (синий), предупреждение (жёлтый), ошибка (красный)
    // - Уведомления автоматически исчезают через указанное время
    // - Реализовано с использованием компонента Bootstrap Toast
    class NotificationContainer : public Wt::WContainerWidget
    {
    public:
        // add: 添加一条新通知。
        // add: добавляет новое уведомление.
        // type: 通知类型（信息/警告/错误）/ тип уведомления (информация/предупреждение/ошибка)
        // message: 通知消息内容 / содержимое сообщения уведомления
        // duration: 显示持续时间（毫秒）/ длительность отображения (миллисекунды)
        void add(Notification::Type type, const Wt::WString& message, std::chrono::milliseconds duration);
    };
} // namespace lms::ui
