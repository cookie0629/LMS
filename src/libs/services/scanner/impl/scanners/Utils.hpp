
#pragma once

#include <filesystem>

#include "database/Object.hpp"

namespace lms::db
{
    class Directory;
    class MediaLibrary;
    class Session;
} // namespace lms::db

namespace lms::scanner::utils
{
    db::ObjectPtr<db::Directory> getOrCreateDirectory(db::Session& session, const std::filesystem::path& path, const db::ObjectPtr<db::MediaLibrary>& mediaLibrary);
} // namespace lms::scanner::utils