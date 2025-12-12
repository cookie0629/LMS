
#pragma once

#include "services/transcoding/ITranscodeService.hpp"

namespace lms::transcoding
{
    class TranscodeService : public ITranscodeService
    {
    public:
        explicit TranscodeService(db::IDb& db, core::IChildProcessManager& childProcessManager);
        ~TranscodeService() override;

        TranscodeService(const TranscodeService&) = delete;
        TranscodeService& operator=(const TranscodeService&) = delete;

    private:
        std::unique_ptr<core::IResourceHandler> createTranscodeResourceHandler(const audio::TranscodeParameters& parameters, bool estimateContentLength) override;

        db::IDb& _db;
        core::IChildProcessManager& _childProcessManager;
    };
} // namespace lms::transcoding
