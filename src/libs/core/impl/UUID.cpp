
#include "core/UUID.hpp"

#include <cassert>
#include <iomanip>
#include <regex>
#include <sstream>

#include "core/Random.hpp"
#include "core/String.hpp"

namespace lms::core
{
    namespace stringUtils
    {
        template<>
        std::optional<UUID>
        readAs(std::string_view str)
        {
            return UUID::fromString(str);
        }
    } // namespace stringUtils
    namespace
    {
        bool stringIsUUID(std::string_view str)
        {
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
        // Form is "123e4567-e89b-12d3-a456-426614174000"
        // TODO: store 128 bits and only convert to string when necessary

        std::ostringstream oss;

        auto concatRandomBytes{ [](std::ostream& os, std::size_t byteCount) {
            for (std::size_t i{}; i < byteCount; ++i)
                os << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(random::getRandom<std::uint8_t>(0, 255));
        } };

        concatRandomBytes(oss, 4);
        oss << "-";
        concatRandomBytes(oss, 2);
        oss << "-";
        concatRandomBytes(oss, 2);
        oss << "-";
        concatRandomBytes(oss, 2);
        oss << "-";
        concatRandomBytes(oss, 6);

        const auto uuid{ fromString(oss.str()) };
        assert(uuid);
        return uuid.value();
    }
} // namespace lms::core