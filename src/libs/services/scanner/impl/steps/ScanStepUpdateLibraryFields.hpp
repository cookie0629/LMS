
#pragma once

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    struct MediaLibraryInfo;

    class ScanStepUpdateLibraryFields : public ScanStepBase
    {
    public:
        using ScanStepBase::ScanStepBase;

    private:
        core::LiteralString getStepName() const override { return "Update Library fields"; }
        ScanStep getStep() const override { return ScanStep::UpdateLibraryFields; }
        bool needProcess(const ScanContext& context) const override;
        void process(ScanContext& context) override;

        void processDirectories(ScanContext& context);
        void processDirectory(ScanContext& context, const MediaLibraryInfo& mediaLibrary);
    };
} // namespace lms::scanner
