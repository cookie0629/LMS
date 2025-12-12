
#pragma once

#include "database/Object.hpp"
#include "database/objects/ArtworkId.hpp"
#include "database/objects/ReleaseId.hpp"

#include "common/Template.hpp"

namespace lms::db
{
    class Medium;
    class Release;
} // namespace lms::db

namespace lms::ui
{
    class Filters;
    class PlayQueueController;

    class Release : public Template
    {
    public:
        Release(Filters& filters, PlayQueueController& playQueueController);

    private:
        void refreshView();
        void refreshArtwork(db::ArtworkId artworkId);
        void refreshReleaseArtists(const db::ObjectPtr<db::Release>& release);
        void refreshDiscs(const db::ObjectPtr<db::Release>& release);
        void createTracks(Wt::WContainerWidget* tracksContainer, const db::ObjectPtr<db::Medium>& medium, bool displayTrackArtists);
        void refreshCopyright(const db::ObjectPtr<db::Release>& release);
        void refreshLinks(const db::ObjectPtr<db::Release>& release);
        void refreshOtherVersions(const db::ObjectPtr<db::Release>& release);
        void refreshSimilarReleases(const std::vector<db::ReleaseId>& similarReleaseIds);

        std::unique_ptr<Wt::WTemplate> createDisc(const db::ObjectPtr<db::Medium>& medium);

        Filters& _filters;
        PlayQueueController& _playQueueController;
        db::ReleaseId _releaseId;
        bool _needForceRefresh{};
    };
} // namespace lms::ui
