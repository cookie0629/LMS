#include "database/Session.hpp"

#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "Db.hpp"
#include "database/Transaction.hpp"
#include "database/Migration.hpp"
#include "database/objects/User.hpp"
#include "database/objects/AuthToken.hpp"
#include "database/objects/Artist.hpp"
#include "database/objects/Release.hpp"
#include "database/objects/Medium.hpp"
#include "database/objects/Track.hpp"
#include "database/objects/MediaLibrary.hpp"
#include "database/objects/Directory.hpp"

namespace lms::db
{
    Session::Session(IDb& db)
        : _db{ db }
    {
        // 设置连接池
        auto& dbImpl = static_cast<Db&>(_db);
        _session.setConnectionPool(dbImpl.getConnectionPool());

        // 映射数据类到表
        _session.mapClass<User>("user");
        _session.mapClass<AuthToken>("auth_token");
        _session.mapClass<VersionInfo>("version_info");
        _session.mapClass<Artist>("artist");
        _session.mapClass<Release>("release");
        _session.mapClass<Medium>("medium");
        _session.mapClass<Track>("track");
        _session.mapClass<MediaLibrary>("media_library");
        _session.mapClass<Directory>("directory");
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
            // 注意：createTables() 需要在事务外执行
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
        return Migration::doDbMigration(*this);
    }
} // namespace lms::db

