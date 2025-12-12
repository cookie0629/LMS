
#pragma once

#include <memory>

#include "IEngine.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::recommendation
{
    std::unique_ptr<IEngine> createFeaturesEngine(db::IDb& db);
}
