
#pragma once

#include <filesystem>
#include <unordered_map>

#include "database/objects/TrackId.hpp"
#include "som/Network.hpp"

namespace lms::recommendation
{

    class FeaturesEngineCache
    {
    public:
        static void invalidate();

        static std::optional<FeaturesEngineCache> read();
        void write() const;

    private:
        using TrackPositions = std::unordered_map<db::TrackId, std::vector<som::Position>>;

        FeaturesEngineCache(som::Network network, TrackPositions trackPositions);

        static std::optional<som::Network> createNetworkFromCacheFile(const std::filesystem::path& path);
        static std::optional<TrackPositions> createObjectPositionsFromCacheFile(const std::filesystem::path& path);
        static bool objectPositionToCacheFile(const TrackPositions& trackPositions, const std::filesystem::path& path);

        friend class FeaturesEngine;

        som::Network _network;
        TrackPositions _trackPositions;
    };

} // namespace lms::recommendation
