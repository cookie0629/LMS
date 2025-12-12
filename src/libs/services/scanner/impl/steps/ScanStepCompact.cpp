
#include "ScanStepCompact.hpp"

#include "database/IDb.hpp"
#include "database/Session.hpp"

#include "ScanContext.hpp"

namespace lms::scanner
{
    bool ScanStepCompact::needProcess(const ScanContext& context) const
    {
        // Don't auto compact as it may be too annoying to block the whole application for very large databases
        return context.scanOptions.compact;
    }

    void ScanStepCompact::process([[maybe_unused]] ScanContext& context)
    {
        _db.getTLSSession().vacuum();
    }
} // namespace lms::scanner
