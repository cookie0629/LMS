#include "database/Migration.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/Utils.hpp"
#include "Db.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::VersionInfo)

namespace lms::db
{
    namespace
    {
        // 当前数据库版本号
        static constexpr Version LMS_DATABASE_VERSION{ 1 };
    }

    VersionInfo::VersionInfo()
        : _version{ static_cast<int>(LMS_DATABASE_VERSION) }
    {
    }

    VersionInfo::pointer VersionInfo::getOrCreate(Session& session)
    {
        session.checkWriteTransaction();

        // 尝试查找现有版本信息
        auto query = session.getDboSession()->find<VersionInfo>();
        auto results = query.resultList();
        
        VersionInfo::pointer versionInfo;
        if (!results.empty())
        {
            versionInfo = results.front();
        }
        else
        {
            // 创建新的版本信息
            versionInfo = session.getDboSession()->add(std::make_unique<VersionInfo>());
        }

        return versionInfo;
    }

    VersionInfo::pointer VersionInfo::get(Session& session)
    {
        session.checkReadTransaction();

        auto query = session.getDboSession()->find<VersionInfo>();
        auto results = query.resultList();
        
        if (!results.empty())
        {
            return results.front();
        }
        
        return VersionInfo::pointer{};
    }
} // namespace lms::db

namespace lms::db::Migration
{
    /**
     * @brief 作用域类：临时禁用外键约束
     */
    class ScopedNoForeignKeys
    {
    public:
        ScopedNoForeignKeys(Db& db)
            : _db{ db }
        {
            _db.executeSql("PRAGMA foreign_keys=OFF");
        }

        ~ScopedNoForeignKeys()
        {
            _db.executeSql("PRAGMA foreign_keys=ON");
        }

        ScopedNoForeignKeys(const ScopedNoForeignKeys&) = delete;
        ScopedNoForeignKeys(ScopedNoForeignKeys&&) = delete;
        ScopedNoForeignKeys& operator=(const ScopedNoForeignKeys&) = delete;
        ScopedNoForeignKeys& operator=(ScopedNoForeignKeys&&) = delete;

    private:
        Db& _db;
    };

    bool doDbMigration(Session& session)
    {
        // 临时禁用外键约束（迁移过程中可能需要）
        auto& dbImpl = static_cast<Db&>(session.getDb());
        ScopedNoForeignKeys noForeignKeys{ dbImpl };

        bool migrationPerformed{ false };

        {
            auto transaction = session.createWriteTransaction();

            Version currentVersion{ 0 };

            try
            {
                // 尝试获取版本信息
                auto versionInfo = VersionInfo::get(session);
                if (versionInfo)
                {
                    currentVersion = versionInfo->getVersion();
                }
                else
                {
                    // 如果版本信息不存在，说明是全新数据库，版本为 0
                    currentVersion = 0;
                }

                LMS_LOG(DB, INFO, "Database version = " << currentVersion 
                    << ", LMS binary version = " << LMS_DATABASE_VERSION);
            }
            catch (const std::exception& e)
            {
                // 如果表不存在，说明是全新数据库，版本为 0
                std::string errorMsg = e.what();
                if (errorMsg.find("no such table") != std::string::npos)
                {
                    currentVersion = 0;
                    LMS_LOG(DB, INFO, "VersionInfo table not found, assuming version 0");
                }
                else
                {
                    LMS_LOG(DB, ERROR, "Cannot get database version info: " << e.what());
                    throw;
                }
            }

            // 检查版本兼容性
            if (currentVersion > LMS_DATABASE_VERSION)
            {
                throw std::runtime_error("Server binary outdated, please upgrade it to handle this database");
            }

            // 如果版本已是最新，无需迁移
            if (currentVersion == LMS_DATABASE_VERSION)
            {
                return false;
            }

            // 执行迁移（从当前版本升级到最新版本）
            // 注意：当前简化版本只支持版本 1，所以如果版本是 0，需要创建初始表结构
            if (currentVersion < LMS_DATABASE_VERSION)
            {
                LMS_LOG(DB, INFO, "Migrating database from version " << currentVersion 
                    << " to " << LMS_DATABASE_VERSION << "...");

                // 对于版本 0 -> 1 的迁移，表结构会在 prepareTablesIfNeeded 中创建
                // 这里需要创建版本信息并设置版本号
                // 先尝试获取，如果不存在则创建
                auto versionInfo = VersionInfo::get(session);
                if (!versionInfo)
                {
                    // 如果不存在，创建新的版本信息
                    versionInfo = session.getDboSession()->add(std::make_unique<VersionInfo>());
                }
                
                versionInfo.modify()->setVersion(LMS_DATABASE_VERSION);

                LMS_LOG(DB, INFO, "Migration complete to version " << LMS_DATABASE_VERSION);

                migrationPerformed = true;
            }
        }

        return migrationPerformed;
    }
} // namespace lms::db::Migration

