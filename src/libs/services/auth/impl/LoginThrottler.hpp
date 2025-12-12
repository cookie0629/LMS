
#pragma once

#include <chrono>
#include <unordered_map>

#include "core/NetAddress.hpp" // for unordered_map of boost::asio::ip::address

namespace lms::auth
{
    class LoginThrottler
    {
    public:
        using Clock = std::chrono::steady_clock;

        LoginThrottler(std::size_t maxEntries)
            : _maxEntries{ maxEntries } {}

        ~LoginThrottler() = default;
        LoginThrottler(const LoginThrottler&) = delete;
        LoginThrottler& operator=(const LoginThrottler&) = delete;

        // user must lock these calls to avoid races
        bool isClientThrottled(const boost::asio::ip::address& address) const;
        void onBadClientAttempt(const boost::asio::ip::address& address);
        void onGoodClientAttempt(const boost::asio::ip::address& address);

    private:
        void removeOutdatedEntries();

        const std::size_t _maxEntries;
        static constexpr std::size_t _maxBadConsecutiveAttemptCount{ 5 };
        static constexpr std::chrono::seconds _throttlingDuration{ 3 };

        struct AttemptInfo
        {
            Clock::time_point nextAttempt{};
            std::size_t badConsecutiveAttemptCount{};
        };
        std::unordered_map<boost::asio::ip::address, AttemptInfo> _attemptsInfo;
    };
} // namespace lms::auth
