#include "core/UUID.hpp"

#include <cassert>
#include <iomanip>
#include <regex>
#include <sstream>

#include "core/Random.hpp"
#include "core/String.hpp"

namespace lms::core
{
    namespace
    {
        /**
         * @brief 检查字符串是否为有效的 UUID 格式
         * @param str 要检查的字符串
         * @return true 如果是有效的 UUID 格式，false 否则
         */
        bool stringIsUUID(std::string_view str)
        {
            // UUID 格式：xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
            static const std::regex re{ R"([0-9a-fA-F]{8}\-[0-9a-fA-F]{4}\-[0-9a-fA-F]{4}\-[0-9a-fA-F]{4}\-[0-9a-fA-F]{12})" };
            
            return std::regex_match(std::cbegin(str), std::cend(str), re);
        }
    } // namespace

    UUID::UUID(std::string_view str)
        : _value{ stringUtils::stringToLower(str) }
    {
    }

    std::optional<UUID> UUID::fromString(std::string_view str)
    {
        if (!stringIsUUID(str))
            return std::nullopt;

        return UUID{ str };
    }

    UUID UUID::generate()
    {
        // UUID v4 格式：xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
        // 其中第13位（第三部分的第一个字符）必须是 '4'
        // 第17位（第四部分的第一个字符）必须是 '8', '9', 'a', 或 'b'
        
        std::ostringstream oss;

        auto concatRandomHex = [](std::ostream& os, std::size_t charCount) {
            for (std::size_t i = 0; i < charCount; ++i)
            {
                os << std::hex << random::getRandom<int>(0, 15);
            }
        };

        // 第一部分：8 个十六进制字符
        concatRandomHex(oss, 8);
        oss << "-";
        
        // 第二部分：4 个十六进制字符
        concatRandomHex(oss, 4);
        oss << "-";
        
        // 第三部分：4 个十六进制字符，第一个字符必须是 '4'（UUID v4）
        oss << "4";
        concatRandomHex(oss, 3);
        oss << "-";
        
        // 第四部分：4 个十六进制字符，第一个字符必须是 '8', '9', 'a', 或 'b'
        int variant = random::getRandom<int>(8, 11);  // 8, 9, 10(a), 11(b)
        oss << std::hex << variant;
        concatRandomHex(oss, 3);
        oss << "-";
        
        // 第五部分：12 个十六进制字符
        concatRandomHex(oss, 12);

        std::string uuidStr = oss.str();
        const auto uuid = fromString(uuidStr);
        assert(uuid.has_value());
        return uuid.value();
    }
} // namespace lms::core

