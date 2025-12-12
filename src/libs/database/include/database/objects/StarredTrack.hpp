
#pragma once

#include <optional>

#include <Wt/Dbo/Field.h>
#include <Wt/WDateTime.h>

#include "database/Object.hpp"
#include "database/Types.hpp"
#include "database/objects/StarredTrackId.hpp"
#include "database/objects/TrackId.hpp"
#include "database/objects/UserId.hpp"

namespace lms::db
{
    class Track;
    class Session;
    class User;

    class StarredTrack final : public Object<StarredTrack, StarredTrackId>
    {
    public:
        StarredTrack() = default;

        struct FindParameters
        {
            std::optional<FeedbackBackend> backend; // for this backend
            std::optional<SyncState> syncState;     //   and these states
            UserId user;                            // and this user
            std::optional<Range> range;

            FindParameters& setFeedbackBackend(FeedbackBackend _backend, SyncState _syncState)
            {
                backend = _backend;
                syncState = _syncState;
                return *this;
            }
            FindParameters& setUser(UserId _user)
            {
                user = _user;
                return *this;
            }
            FindParameters& setRange(std::optional<Range> _range)
            {
                range = _range;
                return *this;
            }
        };

        // Search utility
        static std::size_t getCount(Session& session);
        static pointer find(Session& session, StarredTrackId id);
        static pointer find(Session& session, TrackId trackId, UserId userId); // current feedback backend
        static pointer find(Session& session, TrackId trackId, UserId userId, FeedbackBackend backend);
        static bool exists(Session& session, TrackId trackId, UserId userId, FeedbackBackend backend);
        static RangeResults<StarredTrackId> find(Session& session, const FindParameters& findParams);

        // Accessors
        ObjectPtr<Track> getTrack() const { return _track; }
        ObjectPtr<User> getUser() const { return _user; }
        FeedbackBackend getBackend() const { return _backend; }
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

            Wt::Dbo::belongsTo(a, _track, "track", Wt::Dbo::OnDeleteCascade);
            Wt::Dbo::belongsTo(a, _user, "user", Wt::Dbo::OnDeleteCascade);
        }

    private:
        friend class Session;
        StarredTrack(ObjectPtr<Track> track, ObjectPtr<User> user, FeedbackBackend backend);
        static pointer create(Session& session, ObjectPtr<Track> track, ObjectPtr<User> user, FeedbackBackend backend);

        FeedbackBackend _backend; // for which backend
        SyncState _syncState{ SyncState::PendingAdd };
        Wt::WDateTime _dateTime; // when it was starred

        Wt::Dbo::ptr<Track> _track;
        Wt::Dbo::ptr<User> _user;
    };
} // namespace lms::db
