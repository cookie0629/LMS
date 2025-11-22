#include "LoginThrottler.hpp"

#include <algorithm>

namespace lms::auth
{
    LoginThrottler::LoginThrottler(std::size_t maxEntries)
        : _maxEntries{ maxEntries }
    {
    }

    bool LoginThrottler::isClientThrottled(const boost::asio::ip::address& address) const
    {
        std::shared_lock lock(_mutex);

        removeOutdatedEntries();

        auto it = _entries.find(address);
        if (it == _entries.end())
        {
            return false;
        }

        const auto& entry = it->second;
        const auto now = Clock::now();
        const auto timeSinceLastAttempt = std::chrono::duration_cast<std::chrono::seconds>(now - entry.lastAttempt);

        // 如果连续失败次数超过阈值，且在限流时间内，则限流
        return entry.badAttemptCount >= _maxBadConsecutiveAttemptCount &&
               timeSinceLastAttempt < _throttleDuration;
    }

    void LoginThrottler::onBadClientAttempt(const boost::asio::ip::address& address)
    {
        std::unique_lock lock(_mutex);

        removeOutdatedEntries();

        auto& entry = _entries[address];
        entry.badAttemptCount++;
        entry.lastAttempt = Clock::now();

        // 如果条目数超过最大值，移除最旧的条目
        if (_entries.size() > _maxEntries)
        {
            auto oldestIt = std::min_element(_entries.begin(), _entries.end(),
                [](const auto& a, const auto& b) {
                    return a.second.lastAttempt < b.second.lastAttempt;
                });
            if (oldestIt != _entries.end())
            {
                _entries.erase(oldestIt);
            }
        }
    }

    void LoginThrottler::onGoodClientAttempt(const boost::asio::ip::address& address)
    {
        std::unique_lock lock(_mutex);

        // 成功登录后，移除该地址的记录
        _entries.erase(address);
    }

    void LoginThrottler::removeOutdatedEntries() const
    {
        const auto now = Clock::now();
        auto it = _entries.begin();

        while (it != _entries.end())
        {
            const auto timeSinceLastAttempt = std::chrono::duration_cast<std::chrono::seconds>(now - it->second.lastAttempt);
            
            // 移除超过限流时间的条目
            if (timeSinceLastAttempt >= _throttleDuration)
            {
                it = _entries.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
} // namespace lms::auth

