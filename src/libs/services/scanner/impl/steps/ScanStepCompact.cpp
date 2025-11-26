#include "ScanStepCompact.hpp"

#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"

namespace lms::scanner
{
    ScanStepCompact::ScanStepCompact(db::IDb& db, const ScannerSettings& settings)
        : ScanStepBase{ db, settings }
    {
    }

    bool ScanStepCompact::execute(ScanStats& stats)
    {
        (void)stats;

        if (!getScannerSettings().enableCompact)
        {
            LMS_LOG(SCANNER, INFO, "Compact step disabled, skipping");
            return true;
        }

        LMS_LOG(SCANNER, INFO, "Starting compact database step");

        try
        {
            auto& session = getDb().getTLSSession();

            // VACUUM 不能在显式事务中运行，直接执行即可
            session.execute("VACUUM;");

            LMS_LOG(SCANNER, INFO, "Compact database step completed");
        }
        catch (const std::exception& e)
        {
            LMS_LOG(SCANNER, ERROR, "Compact database step failed: " << e.what());
            return false;
        }

        return true;
    }
} // namespace lms::scanner


