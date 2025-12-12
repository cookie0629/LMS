
#include "RecommendationService.hpp"

#include <vector>

#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/objects/ScanSettings.hpp"

#include "ClustersEngineCreator.hpp"
#include "FeaturesEngineCreator.hpp"

namespace lms::recommendation
{
    namespace
    {
        db::ScanSettings::SimilarityEngineType getSimilarityEngineType(db::Session& session)
        {
            auto transaction{ session.createReadTransaction() };

            return db::ScanSettings::find(session)->getSimilarityEngineType();
        }
    } // namespace

    std::unique_ptr<IRecommendationService> createRecommendationService(db::IDb& db)
    {
        return std::make_unique<RecommendationService>(db);
    }

    RecommendationService::RecommendationService(db::IDb& db)
        : _db{ db }
    {
        load();
    }

    TrackContainer RecommendationService::findSimilarTracks(db::TrackListId trackListId, std::size_t maxCount) const
    {
        TrackContainer res;

        if (!_engine)
            return res;

        return _engine->findSimilarTracksFromTrackList(trackListId, maxCount);
    }

    TrackContainer RecommendationService::findSimilarTracks(const std::vector<db::TrackId>& trackIds, std::size_t maxCount) const
    {
        TrackContainer res;

        if (!_engine)
            return res;

        return _engine->findSimilarTracks(trackIds, maxCount);
    }

    ReleaseContainer RecommendationService::getSimilarReleases(db::ReleaseId releaseId, std::size_t maxCount) const
    {
        ReleaseContainer res;

        if (!_engine)
            return res;

        return _engine->getSimilarReleases(releaseId, maxCount);
        ;
    }

    ArtistContainer RecommendationService::getSimilarArtists(db::ArtistId artistId, core::EnumSet<db::TrackArtistLinkType> linkTypes, std::size_t maxCount) const
    {
        ArtistContainer res;

        if (!_engine)
            return res;

        return _engine->getSimilarArtists(artistId, linkTypes, maxCount);

        return res;
    }

    void RecommendationService::load()
    {
        using namespace db;

        switch (getSimilarityEngineType(_db.getTLSSession()))
        {
        case ScanSettings::SimilarityEngineType::Clusters:
            if (_engineType != EngineType::Clusters)
            {
                _engineType = EngineType::Clusters;
                _engine = createClustersEngine(_db);
            }
            break;

        case ScanSettings::SimilarityEngineType::Features:
        case ScanSettings::SimilarityEngineType::None:
            _engineType.reset();
            _engine.reset();
            break;
        }

        if (_engine)
            _engine->load(false);
    }
} // namespace lms::recommendation
