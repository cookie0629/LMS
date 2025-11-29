#pragma once

#include <filesystem>
#include <memory>

#include "database/objects/TrackEmbeddedImageId.hpp"
#include "services/artwork/IArtworkService.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::artwork
{
    /**
     * @brief Artwork Service 实现（简化版）
     */
    class ArtworkService : public IArtworkService
    {
    public:
        ArtworkService(db::IDb& db,
                      const std::filesystem::path& defaultReleaseCoverSvgPath,
                      const std::filesystem::path& defaultArtistImageSvgPath);
        ~ArtworkService() override = default;
        ArtworkService(const ArtworkService&) = delete;
        ArtworkService& operator=(const ArtworkService&) = delete;

        std::shared_ptr<image::IEncodedImage> getImage(db::ArtworkId artworkId) override;
        std::shared_ptr<image::IEncodedImage> getDefaultReleaseArtwork() override;
        std::shared_ptr<image::IEncodedImage> getDefaultArtistArtwork() override;
        void flushCache() override;

    private:
        std::shared_ptr<image::IEncodedImage> getTrackEmbeddedImage(db::TrackEmbeddedImageId trackEmbeddedImageId);

        db::IDb& _db;
        std::shared_ptr<image::IEncodedImage> _defaultReleaseCover;
        std::shared_ptr<image::IEncodedImage> _defaultArtistImage;
    };
} // namespace lms::artwork

