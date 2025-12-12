
#pragma once

#include <boost/asio/ip/address.hpp>

#ifndef BOOST_ASIO_HAS_STD_HASH
    #include <functional>

namespace std
{
    // Custom hash for boost::asio::ip::address when std::hash is not provided by Boost.
    // 当 Boost 未提供 std::hash 特化时，为 boost::asio::ip::address 提供自定义哈希，用于 unordered_* 容器。
    template<>
    struct hash<boost::asio::ip::address>
    {
        std::size_t operator()(const boost::asio::ip::address& ipAddr) const;
    };

} // namespace std

#endif // BOOST_ASIO_HAS_STD_HASH
