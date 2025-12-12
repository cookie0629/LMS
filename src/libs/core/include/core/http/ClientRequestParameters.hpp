
#pragma once

#include <cstddef>
#include <functional>
#include <span>
#include <vector>

#include <Wt/Http/Message.h>

namespace lms::core::http
{
    // ClientRequestParameters: общие параметры HTTP‑запроса（优先级、路径、回调等）。
    // ClientRequestParameters：HTTP 请求的通用参数（优先级、相对路径、回调等）。
    struct ClientRequestParameters
    {
        enum class Priority
        {
            High,
            Normal,
            Low,
        };

        Priority priority{ Priority::Normal };
        std::string relativeUrl;                            // relative to baseUrl used by the client
        std::size_t responseBufferSize{ 10 * 1024 * 1024 }; // only used if onChunkReceived is not set

        // If `onChunkReceived` is set, the response will be streamed in chunks.
        // In that case, `onSuccessFunc` is still called at the end (with an empty msgBody).
        // If `onChunkReceived` is not set, the response will be fully buffered and passed to `onSuccessFunc`.
        enum class ChunckReceivedResult
        {
            Continue,
            Abort,
        };
        using OnChunkReceived = std::function<ChunckReceivedResult(std::span<const std::byte> chunk)>; // return false to stop (onFailureFunc callback will be called)
        OnChunkReceived onChunkReceived;

        using OnSuccessFunc = std::function<void(const Wt::Http::Message& msg)>;
        OnSuccessFunc onSuccessFunc;

        using OnFailureFunc = std::function<void()>;
        OnFailureFunc onFailureFunc;

        using OnAbortFunc = std::function<void()>;
        OnAbortFunc onAbortFunc;
    };

    struct ClientGETRequestParameters final : public ClientRequestParameters
    {
        std::vector<Wt::Http::Message::Header> headers;
    };

    struct ClientPOSTRequestParameters final : public ClientRequestParameters
    {
        Wt::Http::Message message;
    };

} // namespace lms::core::http