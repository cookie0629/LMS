#pragma once

#include <mutex>
#include <string>
#include <vector>
#include <filesystem>
#include <memory>

#include <Wt/Dbo/SqlConnectionPool.h>

#include "database/IDb.hpp"

namespace lms::db
{
    class Session;

    /**
     * @brief 数据库实现类
     */
    class Db : public IDb
    {
    public:
        /**
         * @brief 构造函数
         * @param dbPath 数据库文件路径
         * @param connectionCount 连接池大小
         */
        Db(const std::filesystem::path& dbPath, std::size_t connectionCount);

        /**
         * @brief 执行 SQL 语句
         * @param sql SQL 语句
         */
        void executeSql(const std::string& sql);

    private:
        Db(const Db&) = delete;
        Db& operator=(const Db&) = delete;

        friend class Session;

        /**
         * @brief 获取线程局部存储的数据库会话
         */
        Session& getTLSSession() override;

        /**
         * @brief 获取连接池
         */
        Wt::Dbo::SqlConnectionPool& getConnectionPool() { return *_connectionPool; }

        /**
         * @brief 记录数据库配置信息
         */
        void logPageSize();
        void logCacheSize();
        void logCompileOptions();

        /**
         * @brief 数据库完整性检查
         */
        void performQuickCheck();
        void performIntegrityCheck();

        /**
         * @brief 作用域连接类
         */
        class ScopedConnection
        {
        public:
            explicit ScopedConnection(Wt::Dbo::SqlConnectionPool& pool);
            ~ScopedConnection();

            Wt::Dbo::SqlConnection* operator->() const;
            Wt::Dbo::SqlConnection& operator*() const { return *_connection; }

        private:
            ScopedConnection(const ScopedConnection&) = delete;
            ScopedConnection& operator=(const ScopedConnection&) = delete;

            Wt::Dbo::SqlConnectionPool& _connectionPool;
            std::unique_ptr<Wt::Dbo::SqlConnection> _connection;
        };

        std::unique_ptr<Wt::Dbo::SqlConnectionPool> _connectionPool;
        std::mutex _tlsSessionsMutex;
        std::vector<std::unique_ptr<Session>> _tlsSessions;
        std::filesystem::path _dbPath;
    };
} // namespace lms::db

