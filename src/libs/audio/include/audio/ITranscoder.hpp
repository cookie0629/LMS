
#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <string_view>

#include "TranscodeTypes.hpp"

namespace lms::audio
{
    class ITranscoder
    {
    public:
        virtual ~ITranscoder() = default;

        // non blocking calls
        using ReadCallback = std::function<void(std::size_t nbReadBytes)>;
        virtual void asyncRead(std::byte* buffer, std::size_t bufferSize, ReadCallback callback) = 0;
        virtual std::size_t readSome(std::byte* buffer, std::size_t bufferSize) = 0;

        virtual std::string_view getOutputMimeType() const = 0;
        virtual const TranscodeOutputParameters& getOutputParameters() const = 0;

        virtual bool finished() const = 0;
    };

    std::unique_ptr<ITranscoder> createTranscoder(const TranscodeParameters& parameters);
} // namespace lms::audio