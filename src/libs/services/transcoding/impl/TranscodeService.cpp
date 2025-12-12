
#include "TranscodeService.hpp"

#include "audio/ITranscoder.hpp"
#include "core/ILogger.hpp"
#include "core/UUID.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"

#include "TranscodeResourceHandler.hpp"

namespace lms::transcoding
{
    namespace
    {
        std::size_t doEstimateContentLength(std::size_t bitrate, std::chrono::milliseconds duration)
        {
            const std::size_t estimatedContentLength{ static_cast<size_t>((bitrate / 8 * duration.count()) / 1000) };
            return estimatedContentLength;
        }
    } // namespace

    std::unique_ptr<ITranscodeService> createTranscodeService(db::IDb& db, core::IChildProcessManager& childProcessManager)
    {
        return std::make_unique<TranscodeService>(db, childProcessManager);
    }

    TranscodeService::TranscodeService(db::IDb& db, core::IChildProcessManager& childProcessManager)
        : _db{ db }
        , _childProcessManager(childProcessManager)
    {
        LMS_LOG(TRANSCODING, INFO, "Service started!");
    }

    TranscodeService::~TranscodeService()
    {
        LMS_LOG(TRANSCODING, INFO, "Service stopped!");
    }

    std::unique_ptr<core::IResourceHandler> TranscodeService::createTranscodeResourceHandler(const audio::TranscodeParameters& parameters, bool estimateContentLength)
    {
        std::optional<std::size_t> estimatedContentLength;

        if (estimateContentLength)
        {
            if (parameters.inputParameters.offset < parameters.inputParameters.duration)
                estimatedContentLength = doEstimateContentLength(*parameters.outputParameters.bitrate, parameters.inputParameters.duration - parameters.inputParameters.offset);
            else
                LMS_LOG(TRANSCODING, WARNING, "Offset " << parameters.inputParameters.offset << " is greater than audio file duration " << parameters.inputParameters.duration << ": not estimating content length");
        }

        return std::make_unique<transcoding::ResourceHandler>(parameters, estimatedContentLength);
    }
} // namespace lms::transcoding
