
#include "CoverArtId.hpp"

#include <cassert>

#include "core/String.hpp"
#include "database/objects/ArtworkId.hpp"

namespace lms::api::subsonic
{
    namespace
    {
        constexpr char timestampSeparatorChar{ '-' };

        std::string idToString(db::ArtworkId id)
        {
            return "art-" + id.toString();
        }
    } // namespace

    std::string idToString(CoverArtId coverId)
    {
        std::string res;

        // produce "art-id-timestamp"
        res = idToString(coverId.id);
        res += timestampSeparatorChar;
        res += std::to_string(coverId.timestamp);

        assert(!res.empty());
        return res;
    }
} // namespace lms::api::subsonic

// Used to parse parameters
namespace lms::core::stringUtils
{
    template<>
    std::optional<api::subsonic::CoverArtId> readAs(std::string_view str)
    {
        std::optional<api::subsonic::CoverArtId> res;

        std::vector<std::string_view> values{ core::stringUtils::splitString(str, '-') };

        // expect "art-id-timestamp"
        if (values.size() != 3)
            return res;

        if (values[0] != "art")
            return res;

        const auto value{ core::stringUtils::readAs<db::ArtworkId::ValueType>(values[1]) };
        const auto timestamp{ core::stringUtils::readAs<std::time_t>(values[2]) };
        if (!value || !timestamp)
            return std::nullopt;

        res.emplace(api::subsonic::CoverArtId{ *value, *timestamp });

        return res;
    }
} // namespace lms::core::stringUtils
