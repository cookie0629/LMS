
#include "SubsonicResourceConfig.hpp"

#include "core/IConfig.hpp"

namespace lms::api::subsonic
{
    namespace
    {
        std::unordered_map<std::string, ProtocolVersion> readConfigProtocolVersions(core::IConfig& config)
        {
            std::unordered_map<std::string, ProtocolVersion> res;

            config.visitStrings("api-subsonic-old-server-protocol-clients",
                                [&](std::string_view client) {
                                    res.emplace(std::string{ client }, ProtocolVersion{ .major = 1, .minor = 12, .patch = 0 });
                                },
                                { "DSub" });

            return res;
        }

        std::unordered_set<std::string> readOpenSubsonicDisabledClients(core::IConfig& config)
        {
            std::unordered_set<std::string> res;

            config.visitStrings("api-open-subsonic-disabled-clients",
                                [&](std::string_view client) {
                                    res.emplace(std::string{ client });
                                },
                                { "DSub" });

            return res;
        }
    } // namespace

    SubsonicResourceConfig readSubsonicResourceConfig(core::IConfig& config)
    {
        return SubsonicResourceConfig{
            .serverProtocolVersionsByClient = readConfigProtocolVersions(config),
            .openSubsonicDisabledClients = readOpenSubsonicDisabledClients(config),
            .supportUserPasswordAuthentication = config.getBool("api-subsonic-support-user-password-auth", true)
        };
    }
} // namespace lms::api::subsonic