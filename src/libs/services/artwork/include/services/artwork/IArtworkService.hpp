
#pragma once

#include <filesystem>
#include <memory>
#include <optional>

#include "database/objects/ArtworkId.hpp"
#include "database/objects/TrackListId.hpp"
#include "image/IEncodedImage.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::artwork
{
    // IArtworkService: 封面图服务接口，负责查找 TrackList/Track/Artist 的封面图并进行缓存。
    // IArtworkService: интерфейс сервиса обложек, ищет/возвращает Artwork для плейлистов, треков и артистов с учётом кэша.
    class IArtworkService
    {
    public:
        virtual ~IArtworkService() = default;

        // 根据 TrackList 选出一个优先使用的封面图。
        // Выбирает предпочтительную обложку для заданного TrackList.
        virtual db::ArtworkId findTrackListImage(db::TrackListId trackListId) = 0;

        // 获取封面图；width 为空表示使用原始尺寸。
        // Получает обложку; если width не задан, используется исходный размер.
        virtual std::shared_ptr<image::IEncodedImage> getImage(db::ArtworkId artworkId, std::optional<image::ImageSize> width) = 0;

        // 默认专辑/艺术家封面（SVG，无固定像素尺寸）。
        // Обложки по умолчанию для релизов/артистов (SVG без фиксированного размера).
        virtual std::shared_ptr<image::IEncodedImage> getDefaultReleaseArtwork() = 0;
        virtual std::shared_ptr<image::IEncodedImage> getDefaultArtistArtwork() = 0;

        virtual void flushCache() = 0;

        virtual void setJpegQuality(unsigned quality) = 0; // 1–100 的 JPEG 输出质量 / качество JPEG в диапазоне 1–100
    };

    std::unique_ptr<IArtworkService> createArtworkService(db::IDb& db, const std::filesystem::path& defaultReleaseCoverSvgPath, const std::filesystem::path& defaultArtistImageSvgPath);

} // namespace lms::artwork
