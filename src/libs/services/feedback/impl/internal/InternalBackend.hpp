
#pragma once

#include "IFeedbackBackend.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::feedback
{
    class InternalBackend final : public IFeedbackBackend
    {
    public:
        InternalBackend(db::IDb& db);
        ~InternalBackend() override = default;
        InternalBackend(const InternalBackend&) = delete;
        InternalBackend& operator=(const InternalBackend&) = delete;

    private:
        void onStarred(db::StarredArtistId artistId) override;
        void onUnstarred(db::StarredArtistId artistId) override;
        void onStarred(db::StarredReleaseId releaseId) override;
        void onUnstarred(db::StarredReleaseId releaseId) override;
        void onStarred(db::StarredTrackId trackId) override;
        void onUnstarred(db::StarredTrackId trackId) override;

        db::IDb& _db;
    };
} // namespace lms::feedback
