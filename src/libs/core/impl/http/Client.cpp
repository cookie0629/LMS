// HTTP 客户端实现（发送请求、处理响应）

#include "Client.hpp"
#include "core/Exception.hpp"

namespace lms::core::http
{
    // createClient: фабрика HTTP‑клиента, скрывает конкретный тип Client за интерфейсом IClient.
    // createClient：HTTP 客户端工厂，通过 IClient 接口隐藏具体的 Client 类型。
    std::unique_ptr<IClient> createClient(boost::asio::io_context& ioContext, std::string_view baseUrl)
    {
        return std::make_unique<Client>(ioContext, baseUrl);
    }

    // sendGETRequest: помещает GET‑запрос в очередь отправки.
    // sendGETRequest：把一个 GET 请求丢进发送队列，由 SendQueue 统一调度。
    void Client::sendGETRequest(ClientGETRequestParameters&& GETParams)
    {
        _sendQueue.sendRequest(std::make_unique<ClientRequest>(std::move(GETParams)));
    }

    // sendPOSTRequest: аналогично sendGETRequest, но для POST.
    // sendPOSTRequest：类似 sendGETRequest，只是封装 POST 请求。
    void Client::sendPOSTRequest(ClientPOSTRequestParameters&& POSTParams)
    {
        _sendQueue.sendRequest(std::make_unique<ClientRequest>(std::move(POSTParams)));
    }

    // abortAllRequests: просит очередь остановить当前所有未完成的请求。
    // abortAllRequests：让发送队列中止当前所有未完成的请求。
    void Client::abortAllRequests()
    {
        _sendQueue.abortAllRequests();
    }
} // namespace lms::core::http