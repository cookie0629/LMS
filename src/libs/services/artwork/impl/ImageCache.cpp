
#include "ImageCache.hpp"

#include <mutex>

#include "core/ILogger.hpp"
#include "core/Random.hpp"

namespace lms::artwork
{
    ImageCache::ImageCache(std::size_t maxCacheSize)
        : _maxCacheSize{ maxCacheSize }
    {
    }

    void ImageCache::addImage(const EntryDesc& entryDesc, std::shared_ptr<image::IEncodedImage> image)
    {
        if (!entryDesc.size)
            return;

        const std::unique_lock lock{ _mutex };

        while (_cacheSize + image->getData().size() > _maxCacheSize && !_cache.empty())
        {
            auto itRandom{ core::random::pickRandom(_cache) };
            _cacheSize -= itRandom->second->getData().size();
            _cache.erase(itRandom);
        }

        _cacheSize += image->getData().size();
        _cache[entryDesc] = image;
    }

    std::shared_ptr<image::IEncodedImage> ImageCache::getImage(const EntryDesc& entryDesc) const
    {
        if (!entryDesc.size)
            return {};

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

        LMS_LOG(COVER, DEBUG, "Cache stats: hits = " << _cacheHits.load() << ", misses = " << _cacheMisses.load() << ", nb entries = " << _cache.size() << ", size = " << _cacheSize);
        _cacheHits = 0;
        _cacheMisses = 0;
        _cacheSize = 0;
        _cache.clear();
    }
} // namespace lms::artwork