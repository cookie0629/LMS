
#include "FileScanOperationBase.hpp"

#include "services/scanner/ScanErrors.hpp"

namespace lms::scanner
{
    FileScanOperationBase::FileScanOperationBase(FileToScan&& fileToScan, db::IDb& db, const ScannerSettings& settings)
        : _file{ std::move(fileToScan) }
        , _db{ db }
        , _settings{ settings }
    {
    }

    FileScanOperationBase::~FileScanOperationBase() = default;
} // namespace lms::scanner