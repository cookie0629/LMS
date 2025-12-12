
#pragma once

#include <atomic>
#include <cassert>
#include <memory>
#include <optional>
#include <shared_mutex>
#include <unordered_map>

#include "database/objects/ArtworkId.hpp"
#include "image/IEncodedImage.hpp"

namespace lms::artwork
{
    // ImageCache: 封面图缓存（按 ArtworkId + 尺寸键入），限制最大内存占用。
    // ImageCache: кэш картинок по ArtworkId и размеру, ограничивает использование памяти.
    class ImageCache
    {
    public:
        ImageCache(std::size_t maxCacheSize);

        struct EntryDesc
        {
            db::ArtworkId id;
            std::optional<std::size_t> size;

            bool operator==(const EntryDesc& other) const = default;
        };

        std::size_t getMaxCacheSize() const { return _maxCacheSize; }

        void addImage(const EntryDesc& entryDesc, std::shared_ptr<image::IEncodedImage> image);
        std::shared_ptr<image::IEncodedImage> getImage(const EntryDesc& entryDesc) const;
        void flush();

    private:
        const std::size_t _maxCacheSize;

        mutable std::shared_mutex _mutex;

        struct EntryHasher
        {
            std::size_t operator()(const EntryDesc& entry) const
            {
                assert(entry.size); // 只缓存已缩放的图片 / кэшируем только изменённые по размеру изображения
                return std::hash<db::ArtworkId>{}(entry.id) ^ std::hash<std::size_t>{}(*entry.size);
            }
        };

        std::unordered_map<EntryDesc, std::shared_ptr<image::IEncodedImage>, EntryHasher> _cache;
        std::size_t _cacheSize{};
        mutable std::atomic<std::size_t> _cacheMisses;
        mutable std::atomic<std::size_t> _cacheHits;
    };
} // namespace lms::artwork