
#pragma once

#include "core/ILogger.hpp"
#include "core/UUID.hpp"
#include "database/objects/UserId.hpp"

#define LOG(sev, message) LMS_LOG(FEEDBACK, sev, "[listenbrainz] " << message)

namespace lms::db
{
    class Session;
}

namespace lms::feedback::listenBrainz::utils
{
    std::optional<core::UUID> getListenBrainzToken(db::Session& session, db::UserId userId);
    std::string parseValidateToken(std::string_view msgBody);
} // namespace lms::feedback::listenBrainz::utils
