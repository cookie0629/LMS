#include "ArtworkService.hpp"

#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include <fstream>

#include "database/objects/Artwork.hpp"
#include "database/objects/Image.hpp"
#include "database/objects/ImageId.hpp"
#include "database/objects/TrackEmbeddedImage.hpp"
#include "database/objects/TrackEmbeddedImageId.hpp"
#include "image/EncodedImage.hpp"
#include "services/artwork/IArtworkService.hpp"

namespace lms::artwork
{
    std::unique_ptr<IArtworkService> createArtworkService(db::IDb& db,
                                                          const std::filesystem::path& defaultReleaseCoverSvgPath,
                                                          const std::filesystem::path& defaultArtistImageSvgPath)
    {
        return std::make_unique<ArtworkService>(db, defaultReleaseCoverSvgPath, defaultArtistImageSvgPath);
    }

    ArtworkService::ArtworkService(db::IDb& db,
                                   const std::filesystem::path& defaultReleaseCoverSvgPath,
                                   const std::filesystem::path& defaultArtistImageSvgPath)
        : _db{ db }
        , _cache{ 50 * 1024 * 1024 } // 50MB 缓存大小
    {
        if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
        {
            LMS_LOG(COVER, INFO, "Default release cover path = " << defaultReleaseCoverSvgPath);
            LMS_LOG(COVER, INFO, "Default artist image path = " << defaultArtistImageSvgPath);
        }

        try
        {
            if (std::filesystem::exists(defaultReleaseCoverSvgPath))
            {
                _defaultReleaseCover = std::make_shared<image::EncodedImage>(defaultReleaseCoverSvgPath, "image/svg+xml");
            }
        }
        catch (const std::exception& e)
        {
            if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
            {
                LMS_LOG(COVER, WARNING, "Cannot load default release cover: " << e.what());
            }
        }

        try
        {
            if (std::filesystem::exists(defaultArtistImageSvgPath))
            {
                _defaultArtistImage = std::make_shared<image::EncodedImage>(defaultArtistImageSvgPath, "image/svg+xml");
            }
        }
        catch (const std::exception& e)
        {
            if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
            {
                LMS_LOG(COVER, WARNING, "Cannot load default artist image: " << e.what());
            }
        }
    }

    std::shared_ptr<image::IEncodedImage> ArtworkService::getImage(db::ArtworkId artworkId)
    {
        if (artworkId.getValue() == 0)
        {
            return nullptr;
        }

        // 先尝试从缓存获取
        ImageCache::EntryDesc cacheKey{ artworkId, std::nullopt };
        if (auto cachedImage = _cache.getImage(cacheKey))
        {
            return cachedImage;
        }

        db::Artwork::UnderlyingId underlyingArtworkId;

        {
            db::Session& session = _db.getTLSSession();
            auto transaction = session.createReadTransaction();

            db::Artwork::pointer artwork = db::Artwork::find(session, artworkId);
            if (!artwork)
            {
                return nullptr;
            }

            underlyingArtworkId = artwork->getUnderlyingId();
        }

        std::shared_ptr<image::IEncodedImage> image;
        if (const auto* trackEmbeddedImageId = std::get_if<db::TrackEmbeddedImageId>(&underlyingArtworkId))
        {
            image = getTrackEmbeddedImage(*trackEmbeddedImageId, std::nullopt);
        }
        else if (const auto* imageId = std::get_if<db::ImageId>(&underlyingArtworkId))
        {
            image = getImageFile(*imageId, std::nullopt);
        }

        // 如果获取到图像，添加到缓存
        if (image)
        {
            _cache.addImage(cacheKey, image);
        }

        return image;
    }

    std::shared_ptr<image::IEncodedImage> ArtworkService::getTrackEmbeddedImage(db::TrackEmbeddedImageId trackEmbeddedImageId, std::optional<image::ImageSize> width)
    {
        std::string imageData;
        std::string mimeType;

        {
            db::Session& session = _db.getTLSSession();
            auto transaction = session.createReadTransaction();

            db::TrackEmbeddedImage::pointer image = db::TrackEmbeddedImage::find(session, trackEmbeddedImageId);
            if (!image)
            {
                return nullptr;
            }

            imageData = image->getData();
            mimeType = image->getMimeType();
        }

        if (imageData.empty())
        {
            return nullptr;
        }

        // 将字符串数据转换为 std::byte span
        std::span<const std::byte> dataSpan{ reinterpret_cast<const std::byte*>(imageData.data()), imageData.size() };
        return std::make_shared<image::EncodedImage>(dataSpan, mimeType);
    }

    std::shared_ptr<image::IEncodedImage> ArtworkService::getDefaultReleaseArtwork()
    {
        return _defaultReleaseCover;
    }

    std::shared_ptr<image::IEncodedImage> ArtworkService::getDefaultArtistArtwork()
    {
        return _defaultArtistImage;
    }

    std::shared_ptr<image::IEncodedImage> ArtworkService::getImageFile(db::ImageId imageId, std::optional<image::ImageSize> width)
    {
        if (imageId.getValue() == 0)
        {
            return nullptr;
        }

        std::filesystem::path imagePath;

        {
            db::Session& session = _db.getTLSSession();
            auto transaction = session.createReadTransaction();

            db::Image::pointer image = db::Image::find(session, imageId);
            if (!image)
            {
                return nullptr;
            }

            imagePath = image->getAbsoluteFilePath();
        }

        if (imagePath.empty() || !std::filesystem::exists(imagePath))
        {
            return nullptr;
        }

        // For now, we just read the file. Resizing logic will be added later.
        std::ifstream file(imagePath, std::ios::binary);
        if (!file)
        {
            return nullptr;
        }

        std::vector<char> charData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        std::vector<std::byte> data;
        data.reserve(charData.size());
        for (char c : charData)
        {
            data.push_back(static_cast<std::byte>(c));
        }
        std::span<const std::byte> dataSpan{ data.data(), data.size() };

        // Try to determine MIME type from file extension
        std::string mimeType = "image/jpeg"; // default
        std::string ext = imagePath.extension().string();
        if (ext == ".png")
            mimeType = "image/png";
        else if (ext == ".gif")
            mimeType = "image/gif";
        else if (ext == ".webp")
            mimeType = "image/webp";

        return std::make_shared<image::EncodedImage>(dataSpan, mimeType);
    }

    void ArtworkService::flushCache()
    {
        _cache.flush();
    }
} // namespace lms::artwork

