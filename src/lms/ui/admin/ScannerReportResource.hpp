#pragma once

#include <Wt/WResource.h>

#include "services/scanner/ScannerStats.hpp"

namespace lms::ui
{

    class ScannerReportResource : public Wt::WResource
    {
    public:
        ScannerReportResource();
        ~ScannerReportResource() override;
        ScannerReportResource(const ScannerReportResource&) = delete;
        ScannerReportResource& operator=(const ScannerReportResource&) = delete;

        void setScanStats(const scanner::ScanStats& stats);
        void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response) override;

    private:
        static Wt::WString formatScanError(const scanner::ScanError* error);
        static Wt::WString duplicateReasonToWString(scanner::DuplicateReason reason);
        std::unique_ptr<scanner::ScanStats> _stats;
    };

} // namespace lms::ui
