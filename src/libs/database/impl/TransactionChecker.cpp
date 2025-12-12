// 事务状态检查与守卫实现

#include "TransactionChecker.hpp"

static_assert(LMS_CHECK_TRANSACTION_ACCESSES, "File should be excluded from build");

#include <cassert>

#include "database/Session.hpp"

namespace lms::db
{
    namespace
    {
        struct StackEntry
        {
            TransactionChecker::TransactionType type;
            const Wt::Dbo::Session* session{};
        };

        static thread_local std::vector<StackEntry> transactionStack;
    } // namespace

    void TransactionChecker::pushWriteTransaction(const Wt::Dbo::Session& session)
    {
        pushTransaction(TransactionType::Write, session);
    }

    void TransactionChecker::pushReadTransaction(const Wt::Dbo::Session& session)
    {
        pushTransaction(TransactionType::Read, session);
    }

    void TransactionChecker::popWriteTransaction(const Wt::Dbo::Session& session)
    {
        popTransaction(TransactionType::Write, session);
    }

    void TransactionChecker::popReadTransaction(const Wt::Dbo::Session& session)
    {
        popTransaction(TransactionType::Read, session);
    }

    void TransactionChecker::pushTransaction(TransactionType type, const Wt::Dbo::Session& session)
    {
        assert(transactionStack.empty() || transactionStack.back().session == &session);
        transactionStack.push_back(StackEntry{ type, &session });
    }

    void TransactionChecker::popTransaction(TransactionType type, const Wt::Dbo::Session& session)
    {
        assert(!transactionStack.empty());
        assert(transactionStack.back().type == type);
        assert(transactionStack.back().session == &session);
        transactionStack.pop_back();
    }

    void TransactionChecker::checkWriteTransaction(const Wt::Dbo::Session& session)
    {
        assert(!transactionStack.empty());
        assert(transactionStack.back().type == TransactionType::Write);
        assert(transactionStack.back().session == &session);
    }

    void TransactionChecker::checkWriteTransaction(const Session& session)
    {
        checkWriteTransaction(*session.getDboSession());
    }

    void TransactionChecker::checkReadTransaction(const Wt::Dbo::Session& session)
    {
        assert(!transactionStack.empty());
        assert(transactionStack.back().session == &session);
    }

    void TransactionChecker::checkReadTransaction(const Session& session)
    {
        checkReadTransaction(*session.getDboSession());
    }
} // namespace lms::db