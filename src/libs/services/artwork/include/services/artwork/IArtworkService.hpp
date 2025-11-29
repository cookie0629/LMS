#pragma once

#include <filesystem>
#include <memory>
#include <optional>

#include "database/objects/ArtworkId.hpp"
#include "image/IEncodedImage.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::artwork
{
    /**
     * @brief Artwork Service 接口
     */
    class IArtworkService
    {
    public:
        virtual ~IArtworkService() = default;

        /**
         * @brief 获取图像（简化版，不支持调整大小）
         * @param artworkId Artwork ID
         * @return 编码图像，如果不存在则返回 nullptr
         */
        virtual std::shared_ptr<image::IEncodedImage> getImage(db::ArtworkId artworkId) = 0;

        /**
         * @brief 获取默认专辑封面
         */
        virtual std::shared_ptr<image::IEncodedImage> getDefaultReleaseArtwork() = 0;

        /**
         * @brief 获取默认艺术家图像
         */
        virtual std::shared_ptr<image::IEncodedImage> getDefaultArtistArtwork() = 0;

        /**
         * @brief 清空缓存
         */
        virtual void flushCache() = 0;
    };

    /**
     * @brief 创建 Artwork Service
     */
    std::unique_ptr<IArtworkService> createArtworkService(db::IDb& db,
                                                          const std::filesystem::path& defaultReleaseCoverSvgPath,
                                                          const std::filesystem::path& defaultArtistImageSvgPath);
} // namespace lms::artwork

