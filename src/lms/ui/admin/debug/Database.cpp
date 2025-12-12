
#include "Database.hpp"

#include <Wt/Http/Response.h>
#include <Wt/Utils.h>
#include <Wt/WDateTime.h>
#include <Wt/WPushButton.h>
#include <Wt/WResource.h>

#include "core/ITraceLogger.hpp"
#include "core/String.hpp"
#include "database/IQueryPlanRecorder.hpp"

namespace lms::ui
{
    namespace
    {
        class QueryPlansReportResource : public Wt::WResource
        {
        public:
            QueryPlansReportResource(const db::IQueryPlanRecorder& recorder)
                : _recorder{ recorder }
            {
            }

            ~QueryPlansReportResource()
            {
                beingDeleted();
            }
            QueryPlansReportResource(const QueryPlansReportResource&) = delete;
            QueryPlansReportResource& operator=(const QueryPlansReportResource&) = delete;

        private:
            void handleRequest(const Wt::Http::Request&, Wt::Http::Response& response)
            {
                response.setMimeType("application/text");

                auto encodeHttpHeaderField = [](const std::string& fieldName, const std::string& fieldValue) {
                    // This implements RFC 5987
                    return fieldName + "*=UTF-8''" + Wt::Utils::urlEncode(fieldValue);
                };

                const std::string cdp{ encodeHttpHeaderField("filename", "LMS_db_query_plans_" + core::stringUtils::toISO8601String(Wt::WDateTime::currentDateTime()) + ".txt") };
                response.addHeader("Content-Disposition", "attachment; " + cdp);

                _recorder.visitQueryPlans([&](std::string_view query, std::string_view plan) {
                    response.out() << query << '\n';
                    response.out() << plan << "\n-------------------------\n";
                });
            }

            const db::IQueryPlanRecorder& _recorder;
        };
    } // namespace

    Database::Database()
        : Wt::WTemplate{ Wt::WString::tr("Lms.Admin.DebugTools.Db.template") }
    {
        addFunction("tr", &Wt::WTemplate::Functions::tr);

        Wt::WPushButton* dumpBtn{ bindNew<Wt::WPushButton>("export-query-plans-btn", Wt::WString::tr("Lms.Admin.DebugTools.Db.export-query-plans")) };

        if (const auto* recorder{ core::Service<db::IQueryPlanRecorder>::get() })
        {
            Wt::WLink link{ std::make_shared<QueryPlansReportResource>(*recorder) };
            link.setTarget(Wt::LinkTarget::NewWindow);
            dumpBtn->setLink(link);
        }
        else
            dumpBtn->setEnabled(false);
    }

} // namespace lms::ui
