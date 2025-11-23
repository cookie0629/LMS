#include "ScanStepBase.hpp"

#include "database/IDb.hpp"

namespace lms::scanner
{
    ScanStepBase::ScanStepBase(db::IDb& db, const ScannerSettings& settings)
        : _db{ db }
        , _settings{ settings }
    {
    }
} // namespace lms::scanner

