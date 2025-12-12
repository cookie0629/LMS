
#pragma once

#include <memory>

#include "database/objects/StarredArtistId.hpp"
#include "database/objects/StarredReleaseId.hpp"
#include "database/objects/StarredTrackId.hpp"

namespace lms::feedback
{
    class IFeedbackBackend
    {
    public:
        virtual ~IFeedbackBackend() = default;

        virtual void onStarred(db::StarredArtistId artistId) = 0;
        virtual void onUnstarred(db::StarredArtistId artistId) = 0;
        virtual void onStarred(db::StarredReleaseId releaseId) = 0;
        virtual void onUnstarred(db::StarredReleaseId releaseId) = 0;
        virtual void onStarred(db::StarredTrackId trackId) = 0;
        virtual void onUnstarred(db::StarredTrackId trackId) = 0;
    };

    std::unique_ptr<IFeedbackBackend> createFeedbackBackend(std::string_view backendName);

} // namespace lms::feedback
