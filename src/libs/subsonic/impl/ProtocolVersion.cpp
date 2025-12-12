
#include "ProtocolVersion.hpp"

namespace lms::core::stringUtils
{
    template<>
    std::optional<api::subsonic::ProtocolVersion> readAs(std::string_view str)
    {
        // Expects "X.Y.Z"
        const auto numbers{ core::stringUtils::splitString(str, '.') };
        if (numbers.size() < 2 || numbers.size() > 3)
            return std::nullopt;

        api::subsonic::ProtocolVersion version;

        auto number{ core::stringUtils::readAs<unsigned>(numbers[0]) };
        if (!number)
            return std::nullopt;
        version.major = *number;

        number = { core::stringUtils::readAs<unsigned>(numbers[1]) };
        if (!number)
            return std::nullopt;
        version.minor = *number;

        if (numbers.size() == 3)
        {
            number = { core::stringUtils::readAs<unsigned>(numbers[2]) };
            if (!number)
                return std::nullopt;
            version.patch = *number;
        }

        return version;
    }
} // namespace lms::core::stringUtils
