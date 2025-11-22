#pragma once

#include <Wt/Dbo/Session.h>
#include <span>
#include <string>
#include <string_view>

namespace lms::db
{
    class IDb;
    class WriteTransaction;
    class ReadTransaction;

    /**
     * @brief 数据库会话类，封装 Wt::Dbo::Session
     */
    class Session
    {
    public:
        /**
         * @brief 构造函数
         * @param db 数据库引用
         */
        explicit Session(IDb& db);
        
        ~Session() = default;
        Session(const Session&) = delete;
        Session& operator=(const Session&) = delete;

        /**
         * @brief 创建写事务
         * @return 写事务对象
         */
        WriteTransaction createWriteTransaction();

        /**
         * @brief 创建读事务
         * @return 读事务对象
         */
        ReadTransaction createReadTransaction();

        /**
         * @brief 检查是否有写事务
         */
        void checkWriteTransaction() const;

        /**
         * @brief 检查是否有读事务
         */
        void checkReadTransaction() const;

        /**
         * @brief 执行 SQL 语句
         * @param statement SQL 语句
         */
        void execute(std::string_view statement);

        /**
         * @brief 准备数据库表（如果不存在则创建）
         */
        void prepareTablesIfNeeded();

        /**
         * @brief 迁移数据库架构（如果需要）
         * @return true 如果执行了迁移，false 否则
         */
        bool migrateSchemaIfNeeded();

        /**
         * @brief 获取 Wt::Dbo::Session 指针
         */
        Wt::Dbo::Session* getDboSession() { return &_session; }
        const Wt::Dbo::Session* getDboSession() const { return &_session; }

        /**
         * @brief 获取数据库引用
         */
        IDb& getDb() { return _db; }

        /**
         * @brief 创建数据对象
         * @tparam Object 对象类型
         * @tparam Args 参数类型
         * @param args 构造参数
         * @return 对象指针
         */
        template<typename Object, typename... Args>
        typename Object::pointer create(Args&&... args)
        {
            checkWriteTransaction();
            typename Object::pointer res{ Object::create(*this, std::forward<Args>(args)...) };
            _session.flush();
            return res;
        }

    private:
        IDb& _db;
        Wt::Dbo::Session _session;
    };
} // namespace lms::db

