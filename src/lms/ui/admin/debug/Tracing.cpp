
#include "Tracing.hpp"

#include <Wt/Http/Response.h>
#include <Wt/Utils.h>
#include <Wt/WDateTime.h>
#include <Wt/WPushButton.h>
#include <Wt/WResource.h>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>

#include "core/ITraceLogger.hpp"
#include "core/String.hpp"

namespace lms::ui
{
    namespace
    {
        class TracingReportResource : public Wt::WResource
        {
        public:
            TracingReportResource(core::tracing::ITraceLogger& traceLogger)
                : _traceLogger{ traceLogger }
            {
            }

            ~TracingReportResource()
            {
                beingDeleted();
            }
            TracingReportResource(const TracingReportResource&) = delete;
            TracingReportResource& operator=(const TracingReportResource&) = delete;

            void handleRequest(const Wt::Http::Request&, Wt::Http::Response& response)
            {
                response.setMimeType("application/gzip");

                auto encodeHttpHeaderField = [](const std::string& fieldName, const std::string& fieldValue) {
                    // This implements RFC 5987
                    return fieldName + "*=UTF-8''" + Wt::Utils::urlEncode(fieldValue);
                };

                const std::string cdp{ encodeHttpHeaderField("filename", "LMS_traces_" + core::stringUtils::toISO8601String(Wt::WDateTime::currentDateTime()) + ".json.gz") };
                response.addHeader("Content-Disposition", "attachment; " + cdp);

                boost::iostreams::filtering_ostream gzipStream;
                gzipStream.push(boost::iostreams::gzip_compressor{});
                gzipStream.push(response.out());

                _traceLogger.dumpCurrentBuffer(gzipStream);
            }

        private:
            core::tracing::ITraceLogger& _traceLogger;
        };
    } // namespace

    Tracing::Tracing()
        : Wt::WTemplate{ Wt::WString::tr("Lms.Admin.DebugTools.Tracing.template") }
    {
        addFunction("tr", &Wt::WTemplate::Functions::tr);

        Wt::WPushButton* dumpBtn{ bindNew<Wt::WPushButton>("export-btn", Wt::WString::tr("Lms.Admin.DebugTools.Tracing.export-current-buffer")) };

        if (auto traceLogger{ core::Service<core::tracing::ITraceLogger>::get() })
        {
            Wt::WLink link{ std::make_shared<TracingReportResource>(*traceLogger) };
            link.setTarget(Wt::LinkTarget::NewWindow);
            dumpBtn->setLink(link);
        }
        else
            dumpBtn->setEnabled(false);
    }

} // namespace lms::ui
