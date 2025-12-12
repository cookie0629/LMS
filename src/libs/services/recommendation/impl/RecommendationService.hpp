
#pragma once

#include <optional>

#include "services/recommendation/IRecommendationService.hpp"

#include "IEngine.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::recommendation
{
    enum class EngineType
    {
        Clusters,
        Features,
    };

    class RecommendationService : public IRecommendationService
    {
    public:
        RecommendationService(db::IDb& db);
        ~RecommendationService() override = default;
        RecommendationService(const RecommendationService&) = delete;
        RecommendationService& operator=(const RecommendationService&) = delete;

    private:
        void load() override;

        TrackContainer findSimilarTracks(db::TrackListId tracklistId, std::size_t maxCount) const override;
        TrackContainer findSimilarTracks(const std::vector<db::TrackId>& trackIds, std::size_t maxCount) const override;
        ReleaseContainer getSimilarReleases(db::ReleaseId releaseId, std::size_t maxCount) const override;
        ArtistContainer getSimilarArtists(db::ArtistId artistId, core::EnumSet<db::TrackArtistLinkType> linkTypes, std::size_t maxCount) const override;

        void setEnginePriorities(const std::vector<EngineType>& engineTypes);
        void clearEngines();
        void loadPendingEngine(EngineType engineType, std::unique_ptr<IEngine> engine, bool forceReload, const ProgressCallback& progressCallback);

        db::IDb& _db;
        std::optional<EngineType> _engineType;
        std::unique_ptr<IEngine> _engine;
    };

} // namespace lms::recommendation
