
#include "ScanStepBase.hpp"

#include "ScanContext.hpp"

namespace lms::scanner
{
    ScanStepBase::ScanStepBase(InitParams& initParams)
        : _settings{ initParams.settings }
        , _progressCallback{ initParams.progressCallback }
        , _abortScan{ initParams.abortScan }
        , _db{ initParams.db }
        , _jobScheduler{ initParams.jobScheduler }
        , _fileScanners(initParams.fileScanners)
        , _cachePath{ initParams.cachePath }
        , _lastScanSettings{ initParams.lastScanSettings }
    {
    }

    ScanStepBase::~ScanStepBase() = default;

    void ScanStepBase::addError(ScanContext& context, std::shared_ptr<ScanError> error)
    {
        error->accept(_scanErrorLogger);

        context.stats.errorsCount++;

        if (context.stats.errors.size() < ScanStats::maxStoredErrorCount)
            context.stats.errors.emplace_back(error);
    }
} // namespace lms::scanner
