#include "database/Session.hpp"

#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "Db.hpp"
#include "database/Transaction.hpp"

namespace lms::db
{
    Session::Session(IDb& db)
        : _db{ db }
    {
        // 设置连接池
        auto& dbImpl = static_cast<Db&>(_db);
        _session.setConnectionPool(dbImpl.getConnectionPool());

        // 映射数据类到表
        // 注意：目前还没有数据对象，所以暂时不映射
        // 后续添加 User 等对象时，在这里添加 mapClass 调用
    }

    WriteTransaction Session::createWriteTransaction()
    {
        return WriteTransaction{ *this };
    }

    ReadTransaction Session::createReadTransaction()
    {
        return ReadTransaction{ *this };
    }

    void Session::checkWriteTransaction() const
    {
        // 简化版本：暂时不实现严格的检查
        // 实际项目中可以使用 TransactionChecker
    }

    void Session::checkReadTransaction() const
    {
        // 简化版本：暂时不实现严格的检查
    }

    void Session::execute(std::string_view statement)
    {
        // 使用连接池执行 SQL
        auto& dbImpl = static_cast<Db&>(_db);
        dbImpl.executeSql(std::string{ statement });
    }

    void Session::prepareTablesIfNeeded()
    {
        if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
        {
            LMS_LOG(DB, INFO, "准备数据库表...");
        }

        try
        {
            // 创建表（如果不存在）
            _session.createTables();
            
            if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
            {
                LMS_LOG(DB, INFO, "数据库表准备完成");
            }
        }
        catch (const std::exception& e)
        {
            if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
            {
                LMS_LOG(DB, ERROR, "创建数据库表失败: " << e.what());
            }
            throw;
        }
    }

    bool Session::migrateSchemaIfNeeded()
    {
        // 简化版本：暂时不实现迁移
        // 实际项目中需要实现版本管理和迁移脚本
        return false;
    }
} // namespace lms::db

