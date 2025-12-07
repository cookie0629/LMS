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

#include "ModalManager.hpp"

#include "core/ILogger.hpp"

namespace lms::ui
{
    // ModalManager 构造函数：初始化模态框管理器，设置关闭信号处理。
    // Конструктор ModalManager: инициализирует менеджер модальных окон, настраивает обработку сигнала закрытия.
    ModalManager::ModalManager()
        : _closed{ this, "closed" }
    {
        // 连接关闭信号：当模态框在客户端关闭时，从 DOM 中移除对应的 widget。
        // Подключаем сигнал закрытия: при закрытии модального окна на клиенте удаляем соответствующий widget из DOM.
        _closed.connect([this](const std::string& id) {
            LMS_LOG(UI, DEBUG, "Received closed for id '" << id << "'");
            for (int i{}; i < count(); ++i)
            {
                Wt::WWidget* widget{ this->widget(i) };
                LMS_LOG(UI, DEBUG, "Widget " << i << ", id = '" << widget->id());
                if (widget->id() == id)
                {
                    removeWidget(widget);
                    break;
                }
            }
        });
    }

    // show: 把一个包含 .modal 元素的 widget 插入 DOM，并通过 Bootstrap JS 显示出来。
    // show: добавляет виджет с .modal в DOM и показывает его через Bootstrap JS.
    // 功能说明：
    // 1. 将模态框 widget 添加到容器中
    // 2. 通过 JavaScript 调用 Bootstrap Modal API 显示模态框
    // 3. 监听 'hidden.bs.modal' 事件，在关闭时触发服务器端清理
    // Описание:
    // 1. Добавляет виджет модального окна в контейнер
    // 2. Вызывает Bootstrap Modal API через JavaScript для отображения
    // 3. Слушает событие 'hidden.bs.modal' и при закрытии запускает очистку на сервере
    void ModalManager::show(std::unique_ptr<Wt::WWidget> modalWidget)
    {
        LMS_LOG(UI, DEBUG, "Want to show, id = " << modalWidget->id());
        Wt::WWidget* modal{ modalWidget.get() };
        addWidget(std::move(modalWidget));

        // 生成 JavaScript 代码来显示 Bootstrap 模态框
        // Генерируем JavaScript-код для отображения модального окна Bootstrap
        std::ostringstream oss;
        oss
            << R"({const modalElementParent = document.getElementById(')" << modal->id() << R"(');)"
            << R"(const modalElement = modalElementParent.getElementsByClassName('modal')[0];)"
            << R"(const modal = bootstrap.Modal.getOrCreateInstance(modalElement,{backdrop:true, keyboard:true, focus:true});)"
            << R"(modal.show();)"
            << R"(modalElement.addEventListener('hidden.bs.modal', function () {)"
            << _closed.createCall({ "'" + modal->id() + "'" })
            << R"(modal.dispose();)"
            << R"(});})";

        LMS_LOG(UI, DEBUG, "Running JS '" << oss.str() << "'");

        modal->doJavaScript(oss.str());
    }

    // dispose: 通过 JavaScript 隐藏并清理模态框（如果它还在显示）。
    // dispose: скрывает и очищает модальное окно через JavaScript (если оно ещё отображается).
    // 注意：模态框可能已经在客户端被移除了，所以需要检查 modal 实例是否存在。
    // Примечание: модальное окно может быть уже удалено на клиенте, поэтому нужно проверить наличие экземпляра modal.
    void ModalManager::dispose(Wt::WWidget* modalWidget)
    {
        std::ostringstream oss;
        oss
            << R"({const modalElementParent = document.getElementById(')" << modalWidget->id() << R"(');)"
            << R"(const modalElement = modalElementParent.getElementsByClassName('modal')[0];)"
            << R"(const modal = bootstrap.Modal.getInstance(modalElement);)"
            << R"(if (modal) { modal.hide(); })" // may already be removed client side
            << R"(})";

        LMS_LOG(UI, DEBUG, "Running JS '" << oss.str() << "'");

        modalWidget->doJavaScript(oss.str());
    }
} // namespace lms::ui
