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

#include "SendQueue.hpp"

#include <algorithm>
#include <latch>

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/ssl/error.hpp>

#include "core/Exception.hpp"
#include "core/ILogger.hpp"
#include "core/ITraceLogger.hpp"
#include "core/String.hpp"

#define LOG(sev, message) LMS_LOG(HTTP, sev, "[Http SendQueue] - " << message)

namespace lms::core::stringUtils
{
    template<>
    std::optional<std::chrono::seconds> readAs(std::string_view str)
    {
        std::optional<std::chrono::seconds> res;

        if (const std::optional<std::size_t> value{ readAs<std::size_t>(str) })
            res = std::chrono::seconds{ *value };

        return res;
    }
} // namespace lms::core::stringUtils

namespace lms::core::http
{
    namespace
    {
        template<typename T>
        std::optional<T> headerReadAs(const Wt::Http::Message& msg, std::string_view headerName)
        {
            std::optional<T> res;

            if (const std::string * headerValue{ msg.getHeader(std::string{ headerName }) })
                res = stringUtils::readAs<T>(*headerValue);

            return res;
        }
    } // namespace

    // Конструктор: настраивает Wt::Http::Client и привязывает его коллбеки к strand‑у.
    // 构造函数：配置 Wt::Http::Client，并把其回调投递到 strand 中串行执行。
    SendQueue::SendQueue(boost::asio::io_context& ioContext, std::string_view baseUrl)
        : _ioContext{ ioContext }
        , _baseUrl{ baseUrl }
        , _abortAllRequests{ false }
        , _state{ State::Idle }
        , _client{ _ioContext }
    {
        _client.setFollowRedirect(true);
        _client.setTimeout(std::chrono::seconds{ 5 });
        _client.bodyDataReceived().connect([this](const std::string& data) {
            boost::asio::post(boost::asio::bind_executor(_strand, [this, data] {
                onClientBodyDataReceived(data);
            }));
        });

        _client.done().connect([this](Wt::AsioWrapper::error_code ec, const Wt::Http::Message& msg) {
            boost::asio::post(boost::asio::bind_executor(_strand, [this, ec, msg = std::move(msg)] {
                onClientDone(ec, msg);
            }));
        });
    }

    SendQueue::~SendQueue()
    {
        abortAllRequests();
    }

    // abortAllRequests: останавливает очередь и ждёт, пока все请求被安全取消。
    // abortAllRequests：停止队列，等待所有请求被安全取消。
    void SendQueue::abortAllRequests()
    {
        LOG(DEBUG, "Aborting all requests...");

        assert(!_abortAllRequests);
        _abortAllRequests = true;

        std::latch abortLatch{ 1 };

        boost::asio::post(boost::asio::bind_executor(_strand, [this, &abortLatch] {
            for (auto& [prio, requests] : _sendQueue)
            {
                while (!requests.empty())
                {
                    std::unique_ptr<ClientRequest> request{ std::move(requests.front()) };
                    requests.pop_front();
                    if (request->getParameters().onAbortFunc)
                        request->getParameters().onAbortFunc();
                }
            }

            if (_state == State::Throttled)
                _throttleTimer.cancel();
            else if (_state == State::Sending)
                _client.abort();

            abortLatch.count_down();
        }));

        abortLatch.wait();

        while (_state != State::Idle)
            std::this_thread::yield();

        _abortAllRequests = false;

        LOG(DEBUG, "All requests aborted!");
    }

    // sendRequest: асинхронно добавляет запрос в очередь（在 strand 中执行）。
    // sendRequest：异步地把请求添加到队列中（在 strand 线程上下文里执行）。
    void SendQueue::sendRequest(std::unique_ptr<ClientRequest> request)
    {
        boost::asio::post(_strand, [this, request = std::move(request)]() mutable {
            if (_abortAllRequests)
            {
                LOG(DEBUG, "Not posting request because abortAllRequests() in progress");
                if (request->getParameters().onAbortFunc)
                    request->getParameters().onAbortFunc();

                return;
            }

            _sendQueue[request->getParameters().priority].emplace_back(std::move(request));

            if (_state == State::Idle)
                sendNextQueuedRequest();
        });
    }

    // sendNextQueuedRequest: 从最高优先级开始取下一个要发送的请求。
    // sendNextQueuedRequest：从最高优先级开始取出下一个要发送的请求。
    void SendQueue::sendNextQueuedRequest()
    {
        assert(_strand.running_in_this_thread());
        assert(!_currentRequest);

        for (auto& [prio, requests] : _sendQueue)
        {
            LOG(DEBUG, "Processing prio " << static_cast<int>(prio) << ", request count = " << requests.size());
            while (!requests.empty())
            {
                std::unique_ptr<ClientRequest> request{ std::move(requests.front()) };
                requests.pop_front();

                if (!sendRequest(*request))
                {
                    if (request->getParameters().onFailureFunc)
                        request->getParameters().onFailureFunc();
                    continue;
                }

                setState(State::Sending);
                _currentRequest = std::move(request);
                return;
            }
        }

        setState(State::Idle);
    }

    // sendRequest(const ClientRequest&): 实际调用 Wt::Http::Client 发送 HTTP 请求。
    // sendRequest(const ClientRequest&): фактическая отправка HTTP‑запроса через Wt::Http::Client.
    bool SendQueue::sendRequest(const ClientRequest& request)
    {
        assert(_strand.running_in_this_thread());

        LMS_SCOPED_TRACE_DETAILED("SendQueue", "SendRequest");

        const std::string url{ _baseUrl + request.getParameters().relativeUrl };
        LOG(DEBUG, "Sending " << (request.getType() == ClientRequest::Type::GET ? "GET" : "POST") << " request to url '" << url << "'");

        _client.setMaximumResponseSize(request.getParameters().onChunkReceived ? 0 : request.getParameters().responseBufferSize);

        bool res{};
        switch (request.getType())
        {
        case ClientRequest::Type::GET:
            res = _client.get(url, request.getGETParameters().headers);
            break;

        case ClientRequest::Type::POST:
            res = _client.post(url, request.getPOSTParameters().message);
            break;
        }

        if (!res)
            LOG(ERROR, "Send failed, bad url or unsupported scheme?");

        return res;
    }

    // onClientBodyDataReceived: поток式接收响应体数据块，并转发给 onChunkReceived 回调。
    // onClientBodyDataReceived: по мере прихода данных вызывает пользовательский коллбек onChunkReceived。
    void SendQueue::onClientBodyDataReceived(const std::string& data)
    {
        assert(_strand.running_in_this_thread());
        assert(_currentRequest);

        if (_currentRequest->getParameters().onChunkReceived)
        {
            const auto byteSpan{ std::as_bytes(std::span{ data.data(), data.size() }) };
            if (_currentRequest->getParameters().onChunkReceived(byteSpan) == ClientRequestParameters::ChunckReceivedResult::Abort)
                _client.abort();
        }
    }

    // onClientDone: общий завершение回调，根据 error/status 分支到 abort / error / success。
    // onClientDone: общий обработчик завершения запроса（успех/ошибка/отмена）。
    void SendQueue::onClientDone(Wt::AsioWrapper::error_code ec, const Wt::Http::Message& msg)
    {
        LMS_SCOPED_TRACE_DETAILED("SendQueue", "OnClientDone");

        assert(_currentRequest);

        LOG(DEBUG, "Client done. ec = " << ec.category().name() << " - " << ec.message() << " (" << ec.value() << "), status = " << msg.status());

        if (_abortAllRequests || ec == boost::asio::error::operation_aborted)
            onClientAborted(std::move(_currentRequest));
        else if (ec && (ec != boost::asio::ssl::error::stream_truncated))
            onClientDoneError(std::move(_currentRequest), ec);
        else
            onClientDoneSuccess(std::move(_currentRequest), msg);
    }

    // onClientAborted: 触发请求的 onAbort 回调，然后继续下一个队列元素。
    // onClientAborted: вызывает onAbort и сразу переходит к следующему запросу.
    void SendQueue::onClientAborted(std::unique_ptr<ClientRequest> request)
    {
        assert(_strand.running_in_this_thread());

        if (request->getParameters().onAbortFunc)
            request->getParameters().onAbortFunc();

        sendNextQueuedRequest();
    }

    // onClientDoneError: 对网络/传输错误做指数式重试 + 限流。
    // onClientDoneError: при сетевых ошибках повторяет запрос с задержкой и ограниченным числом попыток.
    void SendQueue::onClientDoneError(std::unique_ptr<ClientRequest> request, Wt::AsioWrapper::error_code ec)
    {
        assert(_strand.running_in_this_thread());

        LOG(WARNING, "Retry " << request->retryCount << ", client error: '" << ec.message() << "'");

        // may be a network error, try again later
        throttle(_defaultRetryWaitDuration);

        if (request->retryCount++ < _maxRetryCount)
        {
            _sendQueue[request->getParameters().priority].emplace_front(std::move(request));
        }
        else
        {
            LOG(ERROR, "Too many retries, giving up operation and throttle");
            if (request->getParameters().onFailureFunc)
                request->getParameters().onFailureFunc();
        }
    }

    // onClientDoneSuccess: 处理 HTTP 状态码、限流头（X-RateLimit-*），并调用成功/失败回调。
    // onClientDoneSuccess: анализирует HTTP‑статус, заголовки X-RateLimit-* и вызывает onSuccess/onFailure。
    void SendQueue::onClientDoneSuccess(std::unique_ptr<ClientRequest> request, const Wt::Http::Message& msg)
    {
        const ClientRequestParameters& requestParameters{ request->getParameters() };
        bool mustThrottle{};
        if (msg.status() == 429)
        {
            _sendQueue[requestParameters.priority].emplace_front(std::move(request));
            mustThrottle = true;
        }

        const auto remainingCount{ headerReadAs<std::size_t>(msg, "X-RateLimit-Remaining") };
        LOG(DEBUG, "Remaining messages = " << (remainingCount ? *remainingCount : 0));
        if (mustThrottle || (remainingCount && *remainingCount == 0))
        {
            const auto waitDuration{ headerReadAs<std::chrono::seconds>(msg, "X-RateLimit-Reset-In") };
            throttle(waitDuration.value_or(_defaultRetryWaitDuration));
        }

        if (!mustThrottle)
        {
            if (msg.status() == 200)
            {
                if (requestParameters.onSuccessFunc)
                    requestParameters.onSuccessFunc(msg);
            }
            else
            {
                LOG(ERROR, "Send error, status = " << msg.status() << ", body = '" << msg.body() << "'");
                if (requestParameters.onFailureFunc)
                    requestParameters.onFailureFunc();
            }
        }

        if (_state != State::Throttled)
            sendNextQueuedRequest();
    }

    // throttle: 根据服务器返回的限流时间或默认时间，暂停发送新的请求。
    // throttle: приостанавливает отправку запросов на указанное время（реакция на rate limiting）。
    void SendQueue::throttle(std::chrono::seconds requestedDuration)
    {
        const std::chrono::seconds duration{ std::clamp(requestedDuration, _minRetryWaitDuration, _maxRetryWaitDuration) };
        LOG(DEBUG, "Throttling for " << duration.count() << " seconds");

        _throttleTimer.expires_after(duration);
        _throttleTimer.async_wait(boost::asio::bind_executor(_strand, [this](const boost::system::error_code& ec) {
            if (ec == boost::asio::error::operation_aborted)
                LOG(DEBUG, "Throttle aborted");
            else if (ec)
                throw LmsException{ "Throttle timer failure: " + std::string{ ec.message() } };

            setState(State::Idle);
            if (!ec)
                sendNextQueuedRequest();
        }));

        setState(State::Throttled);
    }

    // setState: 更新内部状态，并在日志中打印状态转换，便于调试。
    // setState: 更新内部状态，并在日志中打印状态变更，方便调试。
    void SendQueue::setState(State state)
    {
        assert(_strand.running_in_this_thread());
        if (_state != state)
        {
            LOG(DEBUG, "Changing state to " << (state == State::Idle ? "Idle" : state == State::Sending ? "Sending" :
                                                                                                          "Throttled"));
            _state = state;
        }
    }
} // namespace lms::core::http
