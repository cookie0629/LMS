
#pragma once

#include <boost/crc.hpp>

namespace lms::core
{
    // Crc32Calculator: простой обёрткой над boost::crc_32_type для计算 CRC32 校验和。
    // Crc32Calculator：对 boost::crc_32_type 的简单封装，用于计算 CRC32 校验值。
    class Crc32Calculator
    {
    public:
        void processBytes(const std::byte* _data, std::size_t dataSize)
        {
            _result.process_bytes(_data, dataSize);
        }

        std::uint32_t getResult() const
        {
            return _result.checksum();
        }

    private:
        using Crc32Type = boost::crc_32_type;
        Crc32Type _result;
    };
} // namespace lms::core
