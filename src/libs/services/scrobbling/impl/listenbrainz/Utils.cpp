
#include "Utils.hpp"

#include <Wt/Json/Object.h>
#include <Wt/Json/Parser.h>

#include "database/Session.hpp"
#include "database/objects/User.hpp"

namespace lms::scrobbling::listenBrainz::utils
{
    std::optional<core::UUID> getListenBrainzToken(db::Session& session, db::UserId userId)
    {
        auto transaction{ session.createReadTransaction() };

        const db::User::pointer user{ db::User::find(session, userId) };
        if (!user)
            return std::nullopt;

        return user->getListenBrainzToken();
    }

    std::string parseValidateToken(std::string_view msgBody)
    {
        std::string listenBrainzUserName;

        Wt::Json::ParseError error;
        Wt::Json::Object root;
        if (!Wt::Json::parse(std::string{ msgBody }, root, error))
        {
            LOG(ERROR, "Cannot parse 'validate-token' result: " << error.what());
            return listenBrainzUserName;
        }

        if (!root.get("valid").orIfNull(false))
        {
            LOG(INFO, "Invalid listenbrainz user");
            return listenBrainzUserName;
        }

        listenBrainzUserName = root.get("user_name").orIfNull("");
        return listenBrainzUserName;
    }
} // namespace lms::scrobbling::listenBrainz::utils
