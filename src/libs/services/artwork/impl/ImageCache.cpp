#include "ImageCache.hpp"

#include <mutex>

#include "core/ILogger.hpp"
#include "core/Random.hpp"
#include "core/Service.hpp"

namespace lms::artwork
{
    ImageCache::ImageCache(std::size_t maxCacheSize)
        : _maxCacheSize{ maxCacheSize }
        , _cacheMisses{ 0 }
        , _cacheHits{ 0 }
    {
    }

    void ImageCache::addImage(const EntryDesc& entryDesc, std::shared_ptr<image::IEncodedImage> image)
    {
        if (!image)
        {
            return;
        }

        const std::unique_lock lock{ _mutex };

        const auto imageSize = image->getData().size();

        // 如果单个图像就超过缓存大小，不缓存
        if (imageSize > _maxCacheSize)
        {
            return;
        }

        // 如果缓存已满，随机删除一些条目
        while (_cacheSize + imageSize > _maxCacheSize && !_cache.empty())
        {
            auto itRandom{ core::random::pickRandom(_cache) };
            if (itRandom != std::end(_cache))
            {
                _cacheSize -= itRandom->second->getData().size();
                _cache.erase(itRandom);
            }
        }

        _cacheSize += imageSize;
        _cache[entryDesc] = image;
    }

    std::shared_ptr<image::IEncodedImage> ImageCache::getImage(const EntryDesc& entryDesc) const
    {
        const std::shared_lock lock{ _mutex };

        const auto it{ _cache.find(entryDesc) };
        if (it == std::cend(_cache))
        {
            ++_cacheMisses;
            return nullptr;
        }

        ++_cacheHits;
        return it->second;
    }

    void ImageCache::flush()
    {
        const std::unique_lock lock{ _mutex };

        if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
        {
            LMS_LOG(COVER, DEBUG, "Cache stats: hits = " << _cacheHits.load() << ", misses = " << _cacheMisses.load() << ", nb entries = " << _cache.size() << ", size = " << _cacheSize);
        }

        _cacheHits = 0;
        _cacheMisses = 0;
        _cacheSize = 0;
        _cache.clear();
    }
} // namespace lms::artwork

