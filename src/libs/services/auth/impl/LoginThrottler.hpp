#pragma once

#include <chrono>
#include <unordered_map>
#include <shared_mutex>

#include <boost/asio/ip/address.hpp>

namespace lms::auth
{
    /**
     * @brief 登录限流器
     * 防止暴力破解攻击
     */
    class LoginThrottler
    {
    public:
        using Clock = std::chrono::steady_clock;

        explicit LoginThrottler(std::size_t maxEntries);

        ~LoginThrottler() = default;
        LoginThrottler(const LoginThrottler&) = delete;
        LoginThrottler& operator=(const LoginThrottler&) = delete;

        /**
         * @brief 检查客户端是否被限流
         * @param address 客户端IP地址
         * @return true 如果被限流，false 否则
         */
        bool isClientThrottled(const boost::asio::ip::address& address) const;

        /**
         * @brief 记录失败的登录尝试
         * @param address 客户端IP地址
         */
        void onBadClientAttempt(const boost::asio::ip::address& address);

        /**
         * @brief 记录成功的登录尝试
         * @param address 客户端IP地址
         */
        void onGoodClientAttempt(const boost::asio::ip::address& address);

    private:
        /**
         * @brief 移除过期的条目
         */
        void removeOutdatedEntries() const;

        struct Entry
        {
            std::size_t badAttemptCount{};
            Clock::time_point lastAttempt{ Clock::now() };
        };

        const std::size_t _maxEntries;
        static constexpr std::size_t _maxBadConsecutiveAttemptCount{ 5 };
        static constexpr std::chrono::seconds _throttleDuration{ 60 }; // 限流持续时间：60秒

        mutable std::shared_mutex _mutex;
        mutable std::unordered_map<boost::asio::ip::address, Entry> _entries;
    };
} // namespace lms::auth

