
#pragma once

#include <filesystem>

#include "ScanStepBase.hpp"

namespace lms::scanner
{
    class ScanStepCheckForRemovedFiles : public ScanStepBase
    {
    public:
        using ScanStepBase::ScanStepBase;

    private:
        core::LiteralString getStepName() const override { return "Check for removed files"; }
        ScanStep getStep() const override { return ScanStep::CheckForRemovedFiles; }
        bool needProcess(const ScanContext& context) const override;
        void process(ScanContext& context) override;

        template<typename Object>
        void checkForRemovedFiles(ScanContext& context);

        bool checkFile(const std::filesystem::path& p);
    };
} // namespace lms::scanner
