
#pragma once

#include <array>
#include <memory>
#include <optional>

#include "audio/ITranscoder.hpp"
#include "core/IResourceHandler.hpp"

namespace lms::transcoding
{
    class ResourceHandler final : public core::IResourceHandler
    {
    public:
        ResourceHandler(const audio::TranscodeParameters& parameters, std::optional<std::size_t> estimatedContentLength);
        ~ResourceHandler() override;

        ResourceHandler(const ResourceHandler&) = delete;
        ResourceHandler& operator=(const ResourceHandler&) = delete;

    private:
        Wt::Http::ResponseContinuation* processRequest(const Wt::Http::Request& request, Wt::Http::Response& response) override;
        void abort() override {};

        static constexpr std::size_t _chunkSize{ 262'144 };
        std::optional<std::size_t> _estimatedContentLength;
        std::array<std::byte, _chunkSize> _buffer;
        std::size_t _bytesReadyCount{};
        std::size_t _totalServedByteCount{};
        std::unique_ptr<audio::ITranscoder> _transcoder;
    };
} // namespace lms::transcoding
