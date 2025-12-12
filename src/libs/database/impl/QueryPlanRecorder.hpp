
#pragma once

#include <map>
#include <shared_mutex>
#include <string>

#include <Wt/Dbo/Session.h>

#include "database/IQueryPlanRecorder.hpp"

namespace lms::db
{
    class QueryPlanRecorder : public IQueryPlanRecorder
    {
    public:
        QueryPlanRecorder();
        ~QueryPlanRecorder() override;
        QueryPlanRecorder(const QueryPlanRecorder&) = delete;
        QueryPlanRecorder& operator=(const QueryPlanRecorder&) = delete;

        void visitQueryPlans(const QueryPlanVisitor& visitor) const override;

        void recordQueryPlanIfNeeded(Wt::Dbo::Session& session, const std::string& query);

    private:
        mutable std::shared_mutex _mutex;
        std::map<std::string, std::string> _queryPlans;
    };
} // namespace lms::db
