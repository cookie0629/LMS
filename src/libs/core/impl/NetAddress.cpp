
#include "core/NetAddress.hpp"

#ifndef BOOST_ASIO_HAS_STD_HASH

namespace std
{
    std::size_t hash<boost::asio::ip::address>::operator()(const boost::asio::ip::address& ipAddr) const
    {
        if (ipAddr.is_v4())
            return ipAddr.to_v4().to_ulong();

        if (ipAddr.is_v6())
        {
            const auto& range{ ipAddr.to_v6().to_bytes() };
            std::size_t res{};

            for (auto b : range)
                res ^= std::hash<char>{}(static_cast<char>(b));

            return res;
        }

        return std::hash<std::string>{}(ipAddr.to_string());
    }
} // namespace std

#endif // BOOST_ASIO_HAS_STD_HASH
