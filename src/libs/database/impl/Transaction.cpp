
#include "database/Transaction.hpp"

#include "core/RecursiveSharedMutex.hpp"

#include "TransactionChecker.hpp"

namespace lms::db
{
    WriteTransaction::WriteTransaction(core::RecursiveSharedMutex& mutex, Wt::Dbo::Session& session)
        : _lock{ mutex }
        , _trace{ "Database", core::tracing::Level::Detailed, "WriteTransaction" }
        , _transaction{ session }
    {
#if LMS_CHECK_TRANSACTION_ACCESSES
        TransactionChecker::pushWriteTransaction(_transaction.session());
#endif
    }

    WriteTransaction::~WriteTransaction()
    {
#if LMS_CHECK_TRANSACTION_ACCESSES
        TransactionChecker::popWriteTransaction(_transaction.session());
#endif

        core::tracing::ScopedTrace _trace{ "Database", core::tracing::Level::Detailed, "Commit" };
        _transaction.commit();
    }

    ReadTransaction::ReadTransaction(Wt::Dbo::Session& session)
        : _trace{ "Database", core::tracing::Level::Detailed, "ReadTransaction" }
        , _transaction{ session }
    {
#if LMS_CHECK_TRANSACTION_ACCESSES
        TransactionChecker::pushReadTransaction(_transaction.session());
#endif
    }

    ReadTransaction::~ReadTransaction()
    {
#if LMS_CHECK_TRANSACTION_ACCESSES
        TransactionChecker::popReadTransaction(_transaction.session());
#endif
    }
} // namespace lms::db
