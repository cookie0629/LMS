#include "Db.hpp"

#include <Wt/Dbo/FixedSqlConnectionPool.h>
#include <Wt/Dbo/backend/Sqlite3.h>
#include <Wt/Dbo/Logger.h>

#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "database/Session.hpp"

namespace lms::db
{
    namespace
    {
        /**
         * @brief SQLite 连接类
         */
        class Connection : public Wt::Dbo::backend::Sqlite3
        {
        public:
            explicit Connection(const std::filesystem::path& dbPath)
                : Wt::Dbo::backend::Sqlite3{ dbPath.string() }
                , _dbPath{ dbPath }
            {
                prepare();
            }

            Connection(const Connection& other)
                : Wt::Dbo::backend::Sqlite3{ other }
                , _dbPath{ other._dbPath }
            {
                prepare();
            }

            ~Connection() override = default;

        private:
            Connection& operator=(const Connection&) = delete;
            Connection(Connection&&) = delete;
            Connection& operator=(Connection&&) = delete;

            std::unique_ptr<Wt::Dbo::SqlConnection> clone() const override
            {
                return std::make_unique<Connection>(*this);
            }

            /**
             * @brief 配置 SQLite 连接
             */
            void prepare()
            {
                if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
                {
                    LMS_LOG(DB, DEBUG_LEVEL, "配置 SQLite 连接设置...");
                }

                executeSql("PRAGMA journal_mode=WAL");
                executeSql("PRAGMA synchronous=normal");
                executeSql("PRAGMA foreign_keys=ON");

                if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
                {
                    LMS_LOG(DB, DEBUG_LEVEL, "SQLite 连接配置完成");
                }
            }

            std::filesystem::path _dbPath;
        };
    } // namespace

    Db::Db(const std::filesystem::path& dbPath, std::size_t connectionCount)
        : _dbPath{ dbPath }
    {
        // 创建连接池
        auto connection = std::make_unique<Connection>(dbPath);
        _connectionPool = std::make_unique<Wt::Dbo::FixedSqlConnectionPool>(
            std::move(connection), connectionCount);

        // 记录数据库信息
        logPageSize();
        logCacheSize();
        logCompileOptions();

        // 执行完整性检查
        performQuickCheck();
    }

    void Db::executeSql(const std::string& sql)
    {
        ScopedConnection conn(*_connectionPool);
        conn->executeSql(sql);
    }

    Session& Db::getTLSSession()
    {
        // 使用线程局部存储获取会话
        thread_local Session* tlsSession = nullptr;

        if (tlsSession == nullptr)
        {
            std::lock_guard<std::mutex> lock(_tlsSessionsMutex);
            _tlsSessions.push_back(std::make_unique<Session>(*this));
            tlsSession = _tlsSessions.back().get();
        }

        return *tlsSession;
    }

    void Db::logPageSize()
    {
        ScopedConnection conn(*_connectionPool);
        auto statement = conn->prepareStatement("PRAGMA page_size");
        statement->execute();

        if (statement->nextRow())
        {
            int pageSize{};
            if (statement->getResult(0, &pageSize))
            {
                if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
                {
                    LMS_LOG(DB, INFO, "数据库页大小: " << pageSize << " 字节");
                }
            }
        }
    }

    void Db::logCacheSize()
    {
        ScopedConnection conn(*_connectionPool);
        auto statement = conn->prepareStatement("PRAGMA cache_size");
        statement->execute();

        if (statement->nextRow())
        {
            int cacheSize{};
            if (statement->getResult(0, &cacheSize))
            {
                if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
                {
                    LMS_LOG(DB, INFO, "数据库缓存大小: " << cacheSize << " 页");
                }
            }
        }
    }

    void Db::logCompileOptions()
    {
        ScopedConnection conn(*_connectionPool);
        auto statement = conn->prepareStatement("PRAGMA compile_options");
        statement->execute();

        if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
        {
            LMS_LOG(DB, DEBUG_LEVEL, "SQLite 编译选项:");
            while (statement->nextRow())
            {
                std::string option;
                option.reserve(64);
                if (statement->getResult(0, &option, static_cast<int>(option.capacity())))
                {
                    LMS_LOG(DB, DEBUG_LEVEL, "  - " << option);
                }
            }
        }
    }

    void Db::performQuickCheck()
    {
        ScopedConnection conn(*_connectionPool);
        auto statement = conn->prepareStatement("PRAGMA quick_check");
        statement->execute();

        bool integrityOk = false;
        while (statement->nextRow())
        {
            std::string result;
            result.reserve(32);
            if (statement->getResult(0, &result, static_cast<int>(result.capacity())))
            {
                if (result == "ok")
                {
                    integrityOk = true;
                    break;
                }
                else
                {
                    if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
                    {
                        LMS_LOG(DB, ERROR, "数据库完整性检查失败: " << result);
                    }
                }
            }
        }

        if (integrityOk)
        {
            if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
            {
                LMS_LOG(DB, INFO, "数据库完整性检查通过");
            }
        }
    }

    void Db::performIntegrityCheck()
    {
        performQuickCheck();  // 简化版本，只执行快速检查
    }

    // ScopedConnection 实现
    Db::ScopedConnection::ScopedConnection(Wt::Dbo::SqlConnectionPool& pool)
        : _connectionPool{ pool }
        , _connection{ pool.getConnection() }
    {
    }

    Db::ScopedConnection::~ScopedConnection()
    {
        if (_connection)
        {
            _connectionPool.returnConnection(std::move(_connection));
        }
    }

    Wt::Dbo::SqlConnection* Db::ScopedConnection::operator->() const
    {
        return _connection.get();
    }

    // 工厂函数
    std::unique_ptr<IDb> createDb(const std::filesystem::path& dbPath, std::size_t connectionCount)
    {
        return std::make_unique<Db>(dbPath, connectionCount);
    }
} // namespace lms::db

