#include "ArtworkService.hpp"

#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/Artwork.hpp"
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

        if (const auto* trackEmbeddedImageId = std::get_if<db::TrackEmbeddedImageId>(&underlyingArtworkId))
        {
            return getTrackEmbeddedImage(*trackEmbeddedImageId);
        }

        return nullptr;
    }

    std::shared_ptr<image::IEncodedImage> ArtworkService::getTrackEmbeddedImage(db::TrackEmbeddedImageId trackEmbeddedImageId)
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

    void ArtworkService::flushCache()
    {
        // 简化版：暂无缓存实现
    }
} // namespace lms::artwork

