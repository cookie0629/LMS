// 数据库接口，聚合会话与事务创建

#pragma once

#include <filesystem>
#include <memory>

namespace lms::db
{
    class Session;
    class IDb
    {
    public:
        virtual ~IDb() = default;

        virtual Session& getTLSSession() = 0;
    };

    std::unique_ptr<IDb> createDb(const std::filesystem::path& dbPath, std::size_t connectionCount = 10);
} // namespace lms::db
