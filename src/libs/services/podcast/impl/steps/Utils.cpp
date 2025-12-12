
#include "Utils.hpp"

#include <cassert>
#include <system_error>

#include "core/ILogger.hpp"
#include "core/UUID.hpp"
#include "database/Session.hpp"
#include "database/objects/Artwork.hpp"
#include "database/objects/Image.hpp"
#include "database/objects/Podcast.hpp"
#include "image/Exception.hpp"
#include "image/Image.hpp"
#include "image/Types.hpp"

namespace lms::podcast::utils
{
    std::filesystem::path getPodcastRelativePath(const db::Podcast::pointer& podcast)
    {
        assert(podcast);
        return podcast->getId().toString();
    }

    static std::optional<image::ImageProperties> probeImage(const std::filesystem::path& path)
    {
        try
        {
            return image::probeImage(path);
        }
        catch (const image::Exception& e)
        {
            LMS_LOG(PODCAST, WARNING, "Failed to probe artwork image " << path << ": " << e.what());
            return std::nullopt;
        }
    }

    db::Artwork::pointer createArtworkFromImage(db::Session& session, const std::filesystem::path& filePath, std::string_view mimeType)
    {
        std::error_code ec;
        const auto fileSize{ std::filesystem::file_size(filePath, ec) };
        if (ec)
        {
            LMS_LOG(PODCAST, ERROR, "Failed to get file size of " << filePath << ": " << ec.message());
            return db::Artwork::pointer{};
        }

        db::Image::pointer image{ session.create<db::Image>(filePath) };
        image.modify()->setFileSize(static_cast<std::size_t>(fileSize));
        if (const std::optional<image::ImageProperties> imageProperties{ probeImage(filePath) })
        {
            image.modify()->setWidth(imageProperties->width);
            image.modify()->setHeight(imageProperties->height);
        }
        image.modify()->setLastWriteTime(Wt::WDateTime::currentDateTime());
        image.modify()->setMimeType(mimeType);

        return session.create<db::Artwork>(image);
    }

    std::string generateRandomFileName()
    {
        return std::string{ core::UUID::generate().getAsString() };
    }

    void removeFile(const std::filesystem::path& filePath)
    {
        std::error_code ec;
        std::filesystem::remove(filePath, ec);
        if (ec)
            LMS_LOG(PODCAST, WARNING, "Failed to remove file " << filePath << ": " << ec.message());
        else
            LMS_LOG(PODCAST, DEBUG, "Removed file " << filePath);
    }
} // namespace lms::podcast::utils