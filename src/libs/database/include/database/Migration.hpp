#pragma once

#include <cstddef>
#include <Wt/Dbo/Dbo.h>

namespace lms::db
{
    class Session;

    /**
     * @brief 数据库版本号类型
     */
    using Version = std::size_t;

    /**
     * @brief 版本信息数据对象
     */
    class VersionInfo
    {
    public:
        using pointer = Wt::Dbo::ptr<VersionInfo>;

        VersionInfo();

        /**
         * @brief 获取或创建版本信息
         */
        static pointer getOrCreate(Session& session);

        /**
         * @brief 获取版本信息
         */
        static pointer get(Session& session);

        /**
         * @brief 获取当前版本
         */
        Version getVersion() const { return static_cast<Version>(_version); }

        /**
         * @brief 设置版本
         */
        void setVersion(Version version) { _version = static_cast<int>(version); }

        /**
         * @brief 持久化方法（Wt::Dbo 要求）
         */
        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _version, "db_version");
        }

    private:
        int _version;
    };

    /**
     * @brief 数据库迁移命名空间
     */
    namespace Migration
    {
        /**
         * @brief 执行数据库迁移
         * @return true 如果执行了迁移，false 如果不需要迁移
         */
        bool doDbMigration(Session& session);
    }
} // namespace lms::db

