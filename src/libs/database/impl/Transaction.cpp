#include "database/Transaction.hpp"

#include "database/Session.hpp"

namespace lms::db
{
    WriteTransaction::WriteTransaction(Session& session)
        : _transaction(*session.getDboSession(), true)  // true = write transaction
        , _committed(false)
    {
    }

    WriteTransaction::~WriteTransaction()
    {
        if (!_committed)
        {
            _transaction.rollback();
        }
    }

    void WriteTransaction::commit()
    {
        if (!_committed)
        {
            _transaction.commit();
            _committed = true;
        }
    }

    void WriteTransaction::rollback()
    {
        if (!_committed)
        {
            _transaction.rollback();
            _committed = true;  // 标记为已处理，避免析构时再次回滚
        }
    }

    ReadTransaction::ReadTransaction(Session& session)
        : _transaction(*session.getDboSession(), false)  // false = read transaction
        , _committed(false)
    {
    }

    ReadTransaction::~ReadTransaction()
    {
        if (!_committed)
        {
            _transaction.commit();
        }
    }

    void ReadTransaction::commit()
    {
        if (!_committed)
        {
            _transaction.commit();
            _committed = true;
        }
    }
} // namespace lms::db

