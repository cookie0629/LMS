
#pragma once

#include <boost/asio/io_context.hpp>
#include <memory>
#include <string>

#include "FeedbacksSynchronizer.hpp"
#include "IFeedbackBackend.hpp"

namespace lms::db
{
    class IDb;
}

namespace lms::feedback::listenBrainz
{
    class ListenBrainzBackend final : public IFeedbackBackend
    {
    public:
        ListenBrainzBackend(boost::asio::io_context& ioContext, db::IDb& db);
        ~ListenBrainzBackend() override;

    private:
        ListenBrainzBackend(const ListenBrainzBackend&) = delete;
        ListenBrainzBackend& operator=(const ListenBrainzBackend&) = delete;

        void onStarred(db::StarredArtistId starredArtistId) override;
        void onUnstarred(db::StarredArtistId starredArtistId) override;
        void onStarred(db::StarredReleaseId starredReleaseId) override;
        void onUnstarred(db::StarredReleaseId starredReleaseId) override;
        void onStarred(db::StarredTrackId starredTrackId) override;
        void onUnstarred(db::StarredTrackId starredTrackId) override;

        boost::asio::io_context& _ioContext;
        db::IDb& _db;
        std::string _baseAPIUrl;
        std::unique_ptr<core::http::IClient> _client;
        FeedbacksSynchronizer _feedbacksSynchronizer;
    };
} // namespace lms::feedback::listenBrainz