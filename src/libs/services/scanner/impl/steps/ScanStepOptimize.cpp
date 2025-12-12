
#include "ScanStepOptimize.hpp"

#include "core/ILogger.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"

#include "ScanContext.hpp"

namespace lms::scanner
{
    bool ScanStepOptimize::needProcess(const ScanContext& context) const
    {
        if (context.scanOptions.forceOptimize)
            return true;

        // Don't optimize if there are too few files: it may lead to some indexes not being used
        // and will drastically slow down the scan process when adding more files later
        if (context.stats.getChangesCount() > (context.stats.getTotalFileCount() / 5)
            && context.stats.getTotalFileCount() >= 1'000)
        {
            return true;
        }

        return false;
    }

    void ScanStepOptimize::process(ScanContext& context)
    {
        LMS_LOG(DBUPDATER, INFO, "Database analyze started");

        auto& session{ _db.getTLSSession() };

        std::vector<std::string> entries;
        session.retrieveEntriesToAnalyze(entries);
        context.currentStepStats.totalElems = entries.size();
        _progressCallback(context.currentStepStats);

        for (const std::string& entry : entries)
        {
            if (_abortScan)
                break;

            _db.getTLSSession().analyzeEntry(entry);
            context.currentStepStats.processedElems++;
            _progressCallback(context.currentStepStats);
        }

        LMS_LOG(DBUPDATER, INFO, "Database analyze complete");
    }
} // namespace lms::scanner
