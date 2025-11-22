#pragma once

#include <Wt/Dbo/Transaction.h>

namespace lms::db
{
    class Session;

    /**
     * @brief 写事务类
     */
    class WriteTransaction
    {
    public:
        /**
         * @brief 构造函数，开始写事务
         * @param session 数据库会话
         */
        explicit WriteTransaction(Session& session);

        /**
         * @brief 析构函数，自动提交或回滚
         */
        ~WriteTransaction();

        /**
         * @brief 提交事务
         */
        void commit();

        /**
         * @brief 回滚事务
         */
        void rollback();

        WriteTransaction(const WriteTransaction&) = delete;
        WriteTransaction& operator=(const WriteTransaction&) = delete;

    private:
        Wt::Dbo::Transaction _transaction;
        bool _committed;
    };

    /**
     * @brief 读事务类
     */
    class ReadTransaction
    {
    public:
        /**
         * @brief 构造函数，开始读事务
         * @param session 数据库会话
         */
        explicit ReadTransaction(Session& session);

        /**
         * @brief 析构函数，自动提交
         */
        ~ReadTransaction();

        /**
         * @brief 提交事务
         */
        void commit();

        ReadTransaction(const ReadTransaction&) = delete;
        ReadTransaction& operator=(const ReadTransaction&) = delete;

    private:
        Wt::Dbo::Transaction _transaction;
        bool _committed;
    };
} // namespace lms::db

