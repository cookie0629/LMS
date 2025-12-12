// HTTP 发送队列与重试声明

#pragma once

#include <atomic>
#include <deque>
#include <string_view>

#include <Wt/Http/Client.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/asio/steady_timer.hpp>

#include "ClientRequest.hpp"

namespace lms::core::http
{
    // SendQueue: очередь исходящих HTTP‑запросов, работающая поверх Wt::Http::Client.
    // SendQueue：基于 Wt::Http::Client 的 HTTP 请求发送队列，负责排队、重试和限流。
    class SendQueue
    {
    public:
        // ioContext: IO контекст, в котором будут выполняться все операции отправки/обработки。
        // baseUrl:   базовый URL сервиса, например "https://api.listenbrainz.org"。
        // ioContext：执行所有发送/回调的 IO 上下文。
        // baseUrl：  服务基础地址，例如 "https://api.listenbrainz.org"。
        SendQueue(boost::asio::io_context& ioContext, std::string_view baseUrl);
        ~SendQueue();

        SendQueue(const SendQueue&) = delete;
        SendQueue(const SendQueue&&) = delete;
        SendQueue& operator=(const SendQueue&) = delete;
        SendQueue& operator=(const SendQueue&&) = delete;

        // sendRequest: добавить запрос в очередь, будет выполнен согласно приоритету。
        // sendRequest：把请求加入队列，按优先级和限流策略发送。
        void sendRequest(std::unique_ptr<ClientRequest> request);

        // abortAllRequests: отменяет все текущие и ожидающие запросы, вызывает соответствующие onAbort 回调。
        // abortAllRequests：取消当前和队列中的所有请求，并触发它们的 onAbort 回调。
        void abortAllRequests();

    private:
        void sendNextQueuedRequest();
        bool sendRequest(const ClientRequest& request);
        void onClientBodyDataReceived(const std::string& data);
        void onClientAborted(std::unique_ptr<ClientRequest> request);
        void onClientDone(Wt::AsioWrapper::error_code ec, const Wt::Http::Message& msg);
        void onClientDoneError(std::unique_ptr<ClientRequest> request, Wt::AsioWrapper::error_code ec);
        void onClientDoneSuccess(std::unique_ptr<ClientRequest> request, const Wt::Http::Message& msg);
        void throttle(std::chrono::seconds duration);

        const std::size_t _maxRetryCount{ 2 };
        const std::chrono::seconds _defaultRetryWaitDuration{ 30 };
        const std::chrono::seconds _minRetryWaitDuration{ 1 };
        const std::chrono::seconds _maxRetryWaitDuration{ 300 };

        boost::asio::io_context& _ioContext;
        boost::asio::io_context::strand _strand{ _ioContext }; // protect _state, _sendQueue and _currentRequest
        boost::asio::steady_timer _throttleTimer{ _ioContext };
        const std::string _baseUrl;

        enum class State
        {
            Idle,
            Throttled,
            Sending,
        };
        void setState(State state);
        std::atomic<bool> _abortAllRequests;
        State _state;
        Wt::Http::Client _client;
        std::map<ClientRequestParameters::Priority, std::deque<std::unique_ptr<ClientRequest>>> _sendQueue;
        std::unique_ptr<ClientRequest> _currentRequest;
    };
} // namespace lms::core::http