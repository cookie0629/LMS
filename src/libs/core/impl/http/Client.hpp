/*
 * Copyright (C) 2021 Emeric Poupon
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

#include <shared_mutex>
#include <string>
#include <unordered_map>

#include "core/http/IClient.hpp"

#include "SendQueue.hpp"

namespace lms::core::http
{
    // Client: простая обёртка над SendQueue, реализующая интерфейс IClient.
    // Client：在 SendQueue 之上的简单封装，实现 IClient 接口。
    class Client final : public IClient
    {
    public:
        // ioContext: общий IO контекст Boost.Asio, в котором выполняются HTTP‑операции。
        // baseUrl:   базовый URL, к которому будут добавляться относительные пути из запросов。
        // ioContext：共享的 Boost.Asio IO 上下文，所有 HTTP 操作在其中执行。
        // baseUrl：  基础 URL，后续请求只提供相对路径。
        Client(boost::asio::io_context& ioContext, std::string_view baseUrl)
            : _sendQueue{ ioContext, baseUrl }
        {
        }

    private:
        void sendGETRequest(ClientGETRequestParameters&& request) override;
        void sendPOSTRequest(ClientPOSTRequestParameters&& request) override;
        void abortAllRequests() override;

        SendQueue _sendQueue;
    };
} // namespace lms::core::http