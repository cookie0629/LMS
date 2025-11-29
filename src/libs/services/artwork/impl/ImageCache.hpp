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
    /**
     * @brief 图像缓存（简化版，只缓存原始图像）
     */
    class ImageCache
    {
    public:
        ImageCache(std::size_t maxCacheSize);

        struct EntryDesc
        {
            db::ArtworkId id;
            std::optional<std::size_t> size; // 简化版：暂时不使用

            bool operator==(const EntryDesc& other) const
            {
                return id == other.id && size == other.size;
            }
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
                // 简化版：只使用 ArtworkId 进行哈希
                return std::hash<db::ArtworkId>{}(entry.id);
            }
        };

        std::unordered_map<EntryDesc, std::shared_ptr<image::IEncodedImage>, EntryHasher> _cache;
        std::size_t _cacheSize{};
        mutable std::atomic<std::size_t> _cacheMisses;
        mutable std::atomic<std::size_t> _cacheHits;
    };
} // namespace lms::artwork

