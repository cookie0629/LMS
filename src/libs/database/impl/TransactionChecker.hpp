// 事务状态检查与守卫声明

#pragma once

#if !defined(NDEBUG)
    #define LMS_CHECK_TRANSACTION_ACCESSES 1
#else
    #define LMS_CHECK_TRANSACTION_ACCESSES 0
#endif

#if LMS_CHECK_TRANSACTION_ACCESSES

namespace Wt::Dbo
{
    class Session;
}

namespace lms::db
{
    class Session;

    class TransactionChecker
    {
    public:
        enum class TransactionType
        {
            Read,
            Write,
        };

        static void pushWriteTransaction(const Wt::Dbo::Session& session);
        static void pushReadTransaction(const Wt::Dbo::Session& session);

        static void popWriteTransaction(const Wt::Dbo::Session& session);
        static void popReadTransaction(const Wt::Dbo::Session& session);

        static void checkWriteTransaction(const Wt::Dbo::Session& session);
        static void checkWriteTransaction(const Session& session);
        static void checkReadTransaction(const Wt::Dbo::Session& session);
        static void checkReadTransaction(const Session& session);

    private:
        static void pushTransaction(TransactionType type, const Wt::Dbo::Session& session);
        static void popTransaction(TransactionType type, const Wt::Dbo::Session& session);
    };
} // namespace lms::db

#endif