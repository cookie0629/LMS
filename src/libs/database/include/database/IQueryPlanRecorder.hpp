
#pragma once

#include <functional>
#include <memory>

namespace lms::db
{
    // Due to technical limitations, query plans are recorded globally across all databases.
    // As a result, this class is implemented as a singleton rather than being owned per DB instance.
    class IQueryPlanRecorder
    {
    public:
        virtual ~IQueryPlanRecorder() = default;

        using QueryPlanVisitor = std::function<void(std::string_view query, std::string_view plan)>;
        virtual void visitQueryPlans(const QueryPlanVisitor& visitor) const = 0;
    };

    std::unique_ptr<IQueryPlanRecorder> createQueryPlanRecorder();
} // namespace lms::db
