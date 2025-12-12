
#include "database/Object.hpp"

#include "TransactionChecker.hpp"

namespace lms::db
{
    void ObjectPtrBase::checkWriteTransaction([[maybe_unused]] Wt::Dbo::Session& session)
    {
#if LMS_CHECK_TRANSACTION_ACCESSES
        TransactionChecker::checkWriteTransaction(session);
#endif
    }
} // namespace lms::db
