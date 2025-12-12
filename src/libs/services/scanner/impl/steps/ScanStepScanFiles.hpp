
#pragma once

#include <deque>

#include "ScanStepBase.hpp"

namespace lms::core
{
    class IJob;
}

namespace lms::scanner
{
    class IFileScanOperation;
    struct MediaLibraryInfo;

    class ScanStepScanFiles : public ScanStepBase
    {
    public:
        using ScanStepBase::ScanStepBase;

    private:
        ScanStep getStep() const override { return ScanStep::ScanFiles; }
        core::LiteralString getStepName() const override { return "Scan files"; }
        bool needProcess(const ScanContext& context) const override;
        void process(ScanContext& context) override;

        void process(ScanContext& context, const MediaLibraryInfo& mediaLibrary);
        std::size_t processFileScanOperations(ScanContext& context, std::deque<std::unique_ptr<IFileScanOperation>>& scanOperations, bool forceBatch);
        void processFileScanOperation(ScanContext& context, IFileScanOperation& operation);
    };
} // namespace lms::scanner
