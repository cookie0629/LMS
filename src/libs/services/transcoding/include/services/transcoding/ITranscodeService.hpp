
#pragma once

#include <memory>

#include "audio/TranscodeTypes.hpp"

namespace lms
{
    namespace core
    {
        class IChildProcessManager;
        class IResourceHandler;
    } // namespace core

    namespace db
    {
        class IDb;
    }
} // namespace lms

namespace lms::transcoding
{
    class ITranscodeService
    {
    public:
        virtual ~ITranscodeService() = default;

        virtual std::unique_ptr<core::IResourceHandler> createTranscodeResourceHandler(const audio::TranscodeParameters& parameters, bool estimateContentLength = false) = 0;
    };

    std::unique_ptr<ITranscodeService> createTranscodeService(db::IDb& db, core::IChildProcessManager& childProcessManager);
} // namespace lms::transcoding
