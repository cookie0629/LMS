// HTTP 客户端接口定义

#pragma once

#include <string_view>

#include <boost/asio/io_context.hpp>

#include "core/http/ClientRequestParameters.hpp"

namespace lms::core::http
{
    // IClient: абстракция над HTTP‑клиентом, отправляющим запросы последовательно.
    // IClient：HTTP 客户端接口，按顺序（在同一个 strand 中）发送请求。
    class IClient
    {
    public:
        virtual ~IClient() = default;

        virtual void sendGETRequest(ClientGETRequestParameters&& request) = 0;
        virtual void sendPOSTRequest(ClientPOSTRequestParameters&& request) = 0;

        virtual void abortAllRequests() = 0;
    };

    std::unique_ptr<IClient> createClient(boost::asio::io_context& ioContext, std::string_view baseUrl);
} // namespace lms::core::http