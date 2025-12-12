
#pragma once

#include <map>

#include "database/Object.hpp"
#include "database/objects/ArtistId.hpp"
#include "database/objects/ArtworkId.hpp"
#include "database/objects/ReleaseId.hpp"

#include "ReleaseTypes.hpp"
#include "common/Template.hpp"

namespace lms::db
{
    class Artist;
    class Release;
} // namespace lms::db

namespace lms::ui
{
    class Filters;
    class PlayQueueController;
    class InfiniteScrollingContainer;

    class Artist : public Template
    {
    public:
        Artist(Filters& filters, PlayQueueController& controller);

    private:
        void refreshView();
        void refreshArtwork(db::ArtworkId artworkId);
        void refreshArtistInfo();
        void refreshReleases();
        void refreshAppearsOnReleases();
        void refreshNonReleaseTracks();
        void refreshSimilarArtists(const std::vector<db::ArtistId>& similarArtistsId);
        void refreshLinks(const db::ObjectPtr<db::Artist>& artist);

        struct ReleaseContainer;
        void addSomeReleases(ReleaseContainer& releaseContainer);
        bool addSomeNonReleaseTracks();
        static constexpr std::size_t _releasesBatchSize{ 6 };
        static constexpr std::size_t _tracksBatchSize{ 6 };
        static constexpr std::size_t _tracksMaxCount{ 160 };

        Filters& _filters;
        PlayQueueController& _playQueueController;

        // Display releases the same way as MusicBrainz
        struct ReleaseContainer
        {
            InfiniteScrollingContainer* container{};
            std::vector<db::ReleaseId> releases;
        };
        std::map<ReleaseType, ReleaseContainer> _releaseContainers;
        ReleaseContainer _appearsOnReleaseContainer{};
        InfiniteScrollingContainer* _trackContainer{};
        db::ArtistId _artistId{};
        bool _needForceRefresh{};
    };
} // namespace lms::ui
