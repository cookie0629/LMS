// 收藏艺人关系实体定义

#pragma once

#include <Wt/Dbo/Field.h>
#include <Wt/WDateTime.h>

#include "database/Object.hpp"
#include "database/Types.hpp"
#include "database/objects/ArtistId.hpp"
#include "database/objects/StarredArtistId.hpp"
#include "database/objects/UserId.hpp"

namespace lms::db
{
    class Artist;
    class Session;
    class User;

    class StarredArtist final : public Object<StarredArtist, StarredArtistId>
    {
    public:
        StarredArtist() = default;

        // Search utility
        static std::size_t getCount(Session& session);
        static pointer find(Session& session, StarredArtistId id);
        static pointer find(Session& session, ArtistId artistId, UserId userId); // current backend
        static pointer find(Session& session, ArtistId artistId, UserId userId, FeedbackBackend backend);

        // Accessors
        ObjectPtr<Artist> getArtist() const { return _artist; }
        ObjectPtr<User> getUser() const { return _user; }
        FeedbackBackend getFeedbackBackend() const { return _backend; }
        const Wt::WDateTime& getDateTime() const { return _dateTime; }
        SyncState getSyncState() const { return _syncState; }

        // Setters
        void setDateTime(const Wt::WDateTime& dateTime);
        void setSyncState(SyncState state) { _syncState = state; }

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _backend, "backend");
            Wt::Dbo::field(a, _syncState, "sync_state");
            Wt::Dbo::field(a, _dateTime, "date_time");

            Wt::Dbo::belongsTo(a, _artist, "artist", Wt::Dbo::OnDeleteCascade);
            Wt::Dbo::belongsTo(a, _user, "user", Wt::Dbo::OnDeleteCascade);
        }

    private:
        friend class Session;
        StarredArtist(ObjectPtr<Artist> artist, ObjectPtr<User> user, FeedbackBackend scrobblingbackend);
        static pointer create(Session& session, ObjectPtr<Artist> artist, ObjectPtr<User> user, FeedbackBackend scrobblingbackend);

        FeedbackBackend _backend; // for which backend
        SyncState _syncState{ SyncState::PendingAdd };
        Wt::WDateTime _dateTime; // when it was starred

        Wt::Dbo::ptr<Artist> _artist;
        Wt::Dbo::ptr<User> _user;
    };
} // namespace lms::db
