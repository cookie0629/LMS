#include "ScanStepOptimize.hpp"

#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"

namespace lms::scanner
{
    ScanStepOptimize::ScanStepOptimize(db::IDb& db, const ScannerSettings& settings)
        : ScanStepBase{ db, settings }
    {
    }

    bool ScanStepOptimize::execute(ScanStats& stats)
    {
        (void)stats;

        if (!getScannerSettings().enableOptimize)
        {
            LMS_LOG(SCANNER, INFO, "Optimize step disabled, skipping");
            return true;
        }

        LMS_LOG(SCANNER, INFO, "Starting optimize database step");

        try
        {
            auto& session = getDb().getTLSSession();

            // PRAGMA optimize 在 SQLite 中会根据统计信息进行自动优化
            session.execute("PRAGMA optimize;");
            session.execute("ANALYZE;");

            LMS_LOG(SCANNER, INFO, "Optimize database step completed");
        }
        catch (const std::exception& e)
        {
            LMS_LOG(SCANNER, ERROR, "Optimize database step failed: " << e.what());
            return false;
        }

        return true;
    }
} // namespace lms::scanner


