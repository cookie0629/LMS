#pragma once

#include <filesystem>
#include <memory>

namespace lms::db
{
    class Session;

    /**
     * @brief 数据库接口
     */
    class IDb
    {
    public:
        virtual ~IDb() = default;

        /**
         * @brief 获取线程局部存储的数据库会话
         * @return 数据库会话引用
         */
        virtual Session& getTLSSession() = 0;
    };

    /**
     * @brief 创建数据库对象
     * @param dbPath 数据库文件路径
     * @param connectionCount 连接池大小（默认10）
     * @return 数据库对象指针
     */
    std::unique_ptr<IDb> createDb(const std::filesystem::path& dbPath, std::size_t connectionCount = 10);
} // namespace lms::db

