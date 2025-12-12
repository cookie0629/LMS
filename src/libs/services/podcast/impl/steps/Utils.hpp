
#pragma once

#include <filesystem>
#include <string>

#include "database/Object.hpp"

namespace lms::db
{
    class Artwork;
    class Session;
} // namespace lms::db

namespace lms::podcast::utils
{
    db::ObjectPtr<db::Artwork> createArtworkFromImage(db::Session& session, const std::filesystem::path& filePath, std::string_view mimeType);
    std::string generateRandomFileName();
    void removeFile(const std::filesystem::path& filePath);
} // namespace lms::podcast::utils