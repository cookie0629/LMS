#pragma once

#include <memory>

#include <Wt/WResource.h>

namespace lms::db
{
    class IDb;
}

namespace lms::api::subsonic
{
    std::unique_ptr<Wt::WResource> createSubsonicResource(db::IDb& db);
} // namespace lms::api::subsonic
