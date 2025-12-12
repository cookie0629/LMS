
#include "MediaLibraryScanning.hpp"

#include "core/Service.hpp"
#include "services/scanner/IScannerService.hpp"

namespace lms::api::subsonic::Scan
{
    using namespace scanner;

    namespace
    {
        Response::Node createStatusResponseNode()
        {
            Response::Node statusResponse;

            const IScannerService::Status scanStatus{ core::Service<IScannerService>::get()->getStatus() };

            statusResponse.setAttribute("scanning", scanStatus.currentState == IScannerService::State::InProgress);
            if (scanStatus.currentState == IScannerService::State::InProgress)
            {
                std::size_t count{};

                if (scanStatus.currentScanStepStats && scanStatus.currentScanStepStats->currentStep == ScanStep::ScanFiles)
                    count = scanStatus.currentScanStepStats->processedElems;

                statusResponse.setAttribute("count", count);
            }

            return statusResponse;
        }
    } // namespace

    Response handleGetScanStatus(RequestContext& context)
    {
        Response response{ Response::createOkResponse(context.getServerProtocolVersion()) };
        response.addNode("scanStatus", createStatusResponseNode());

        return response;
    }

    Response handleStartScan(RequestContext& context)
    {
        core::Service<IScannerService>::get()->requestImmediateScan();

        Response response{ Response::createOkResponse(context.getServerProtocolVersion()) };
        response.addNode("scanStatus", createStatusResponseNode());

        return response;
    }
} // namespace lms::api::subsonic::Scan
