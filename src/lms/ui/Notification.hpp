
#pragma once

namespace lms::ui::Notification
{
    // Type: 通知类型（信息 / 警告 / 危险），决定 Toast 的颜色样式。
    // Type: тип уведомления (информация / предупреждение / ошибка), определяет цвет Toast‑сообщения.
    enum class Type
    {
        Info,
        Warning,
        Danger,
    };
} // namespace lms::ui::Notification
