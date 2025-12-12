
#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "ProtocolVersion.hpp"

namespace lms::core
{
    class IConfig;
}

namespace lms::api::subsonic
{
    struct SubsonicResourceConfig
    {
        std::unordered_map<std::string, ProtocolVersion> serverProtocolVersionsByClient;
        std::unordered_set<std::string> openSubsonicDisabledClients;
        bool supportUserPasswordAuthentication;
    };

    SubsonicResourceConfig readSubsonicResourceConfig(core::IConfig& _config);
} // namespace lms::api::subsonic