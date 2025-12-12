// 事务封装接口定义

#pragma once

#include <mutex>

#include <Wt/Dbo/Transaction.h>

#include "core/ITraceLogger.hpp"

namespace lms::core
{
    class RecursiveSharedMutex;
}

namespace lms::db
{
    class WriteTransaction
    {
    public:
        ~WriteTransaction();

    private:
        friend class Session;
        WriteTransaction(core::RecursiveSharedMutex& mutex, Wt::Dbo::Session& session);

        WriteTransaction(const WriteTransaction&) = delete;
        WriteTransaction& operator=(const WriteTransaction&) = delete;

        const std::unique_lock<core::RecursiveSharedMutex> _lock;
        const core::tracing::ScopedTrace _trace; // before actual transaction
        Wt::Dbo::Transaction _transaction;
    };

    class ReadTransaction
    {
    public:
        ~ReadTransaction();

    private:
        friend class Session;
        ReadTransaction(Wt::Dbo::Session& session);

        ReadTransaction(const ReadTransaction&) = delete;
        ReadTransaction& operator=(const ReadTransaction&) = delete;

        const core::tracing::ScopedTrace _trace; // before actual transaction
        Wt::Dbo::Transaction _transaction;
    };
} // namespace lms::db