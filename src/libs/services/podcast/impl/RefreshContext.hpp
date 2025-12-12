
#pragma once

#include <filesystem>

namespace lms
{
    namespace db
    {
        class IDb;
    }
    namespace core::http
    {
        class IClient;
    }
} // namespace lms

namespace lms::podcast
{
    class Executor;

    struct RefreshContext
    {
        RefreshContext(Executor& executor, db::IDb& db, core::http::IClient& client, const std::filesystem::path& cachePath)
            : executor{ executor }
            , client{ client }
            , db{ db }
            , cachePath{ cachePath }
            , tmpCachePath{ cachePath / "tmp" }
        {
        }
        ~RefreshContext() = default;
        RefreshContext(const RefreshContext&) = delete;
        RefreshContext& operator=(const RefreshContext&) = delete;

        Executor& executor;
        core::http::IClient& client;
        db::IDb& db;
        const std::filesystem::path cachePath;
        const std::filesystem::path tmpCachePath;
    };
} // namespace lms::podcast