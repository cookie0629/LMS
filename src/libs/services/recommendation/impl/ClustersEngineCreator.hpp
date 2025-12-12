
#pragma once

#include <memory>

namespace lms::db
{
    class IDb;
}

namespace lms::recommendation
{
    class IEngine;
    std::unique_ptr<IEngine> createClustersEngine(db::IDb& db);
} // namespace lms::recommendation
