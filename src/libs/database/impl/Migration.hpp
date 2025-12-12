// 数据库迁移脚本执行与版本管理声明

#pragma once

#include <Wt/Dbo/Dbo.h>

namespace lms::db
{
    class Session;

    using Version = std::size_t;

    class VersionInfo
    {
    public:
        using pointer = Wt::Dbo::ptr<VersionInfo>;

        VersionInfo();

        static VersionInfo::pointer getOrCreate(Session& session);
        static VersionInfo::pointer get(Session& session);

        Version getVersion() const { return _version; }
        void setVersion(Version version) { _version = static_cast<int>(version); }

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _version, "db_version");
        }

    private:
        int _version;
    };

    namespace Migration
    {
        bool doDbMigration(Session& session); // return true if migration was performed
    }
} // namespace lms::db
