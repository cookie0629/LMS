// 可重入读写锁接口

#pragma once

#include <mutex>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

namespace lms::core
{
    // API compatible with shared_mutex
    class RecursiveSharedMutex
    {
    public:
        void lock();
        void unlock();

        void lock_shared();
        void unlock_shared();

#ifndef NDEBUG
        bool isSharedLocked();
        bool isUniqueLocked();
#endif // NDEBUG
    private:
        std::shared_mutex _mutex;
        std::thread::id _uniqueOwner;
        std::size_t _uniqueCount{};

        std::mutex _sharedCountMutex;
        std::unordered_map<std::thread::id, std::size_t> _sharedCounts;
    };
} // namespace lms::core