
#pragma once

#include <optional>

#include <Wt/Dbo/Field.h>
#include <Wt/WDateTime.h>

#include "database/Object.hpp"
#include "database/Types.hpp"
#include "database/objects/ArtistId.hpp"
#include "database/objects/Filters.hpp"
#include "database/objects/ListenId.hpp"
#include "database/objects/ReleaseId.hpp"
#include "database/objects/TrackId.hpp"
#include "database/objects/UserId.hpp"

namespace lms::db
{
    class Session;
    class Track;
    class User;

    class Listen final : public Object<Listen, ListenId>
    {
    public:
        Listen() = default;

        struct FindParameters
        {
            UserId user;
            std::optional<ScrobblingBackend> backend;
            std::optional<SyncState> syncState;
            std::optional<Range> range;

            FindParameters& setUser(UserId _user)
            {
                user = _user;
                return *this;
            }
            FindParameters& setScrobblingBackend(ScrobblingBackend _backend)
            {
                backend = _backend;
                return *this;
            }
            FindParameters& setSyncState(SyncState _syncState)
            {
                syncState = _syncState;
                return *this;
            }
            FindParameters& setRange(Range _range)
            {
                range = _range;
                return *this;
            }
        };

        // Accessors
        static std::size_t getCount(Session& session);
        static pointer find(Session& session, ListenId id);
        static pointer find(Session& session, UserId userId, TrackId trackId, ScrobblingBackend backend, const Wt::WDateTime& dateTime);
        static RangeResults<ListenId> find(Session& session, const FindParameters& parameters);

        // Stats
        struct StatsFindParameters
        {
            UserId user;
            std::optional<ScrobblingBackend> backend;
            db::Filters filters;
            std::vector<std::string_view> keywords; // if non empty, name must match all of these keywords
            std::optional<Range> range;
            ArtistId artist; // if set, matching this artist

            StatsFindParameters& setUser(UserId _user)
            {
                user = _user;
                return *this;
            }
            StatsFindParameters& setScrobblingBackend(std::optional<ScrobblingBackend> _backend)
            {
                backend = _backend;
                return *this;
            }
            StatsFindParameters& setFilters(const db::Filters& _filters)
            {
                filters = _filters;
                return *this;
            }
            StatsFindParameters& setKeywords(const std::vector<std::string_view>& _keywords)
            {
                keywords = _keywords;
                return *this;
            }
            StatsFindParameters& setRange(std::optional<Range> _range)
            {
                range = _range;
                return *this;
            }
            StatsFindParameters& setArtist(ArtistId _artist)
            {
                artist = _artist;
                return *this;
            }
        };

        struct ArtistStatsFindParameters : public StatsFindParameters
        {
            std::optional<TrackArtistLinkType> linkType; // if set, only artists that have produced at least one track with this link type

            ArtistStatsFindParameters& setLinkType(std::optional<TrackArtistLinkType> _linkType)
            {
                linkType = _linkType;
                return *this;
            }
        };

        static RangeResults<ArtistId> getTopArtists(Session& session, const ArtistStatsFindParameters& params);
        static RangeResults<ReleaseId> getTopReleases(Session& session, const StatsFindParameters& params);
        static RangeResults<TrackId> getTopTracks(Session& session, const StatsFindParameters& params);

        static RangeResults<ArtistId> getRecentArtists(Session& session, const ArtistStatsFindParameters& params);
        static RangeResults<ReleaseId> getRecentReleases(Session& session, const StatsFindParameters& params);
        static RangeResults<TrackId> getRecentTracks(Session& session, const StatsFindParameters& params);

        static std::size_t getCount(Session& session, UserId userId, TrackId trackId);   // for the current backend
        static std::size_t getCount(Session& session, UserId userId, ReleaseId trackId); // for the current backend

        static pointer getMostRecentListen(Session& session, UserId userId, ScrobblingBackend backend, ReleaseId releaseId);
        static pointer getMostRecentListen(Session& session, UserId userId, ScrobblingBackend backend, TrackId releaseId);

        SyncState getSyncState() const { return _syncState; }
        ObjectPtr<User> getUser() const { return _user; }
        ObjectPtr<Track> getTrack() const { return _track; }
        const Wt::WDateTime& getDateTime() const { return _dateTime; }

        void setSyncState(SyncState state) { _syncState = state; }

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _dateTime, "date_time");
            Wt::Dbo::field(a, _backend, "backend");      // TODO rename
            Wt::Dbo::field(a, _syncState, "sync_state"); // TODO rename

            Wt::Dbo::belongsTo(a, _track, "track", Wt::Dbo::OnDeleteCascade);
            Wt::Dbo::belongsTo(a, _user, "user", Wt::Dbo::OnDeleteCascade);
        }

    private:
        friend class Session;
        Listen(ObjectPtr<User> user, ObjectPtr<Track> track, ScrobblingBackend backend, const Wt::WDateTime& dateTime);
        static pointer create(Session& session, ObjectPtr<User> user, ObjectPtr<Track> track, ScrobblingBackend backend, const Wt::WDateTime& dateTime);

        Wt::WDateTime _dateTime;
        ScrobblingBackend _backend;
        SyncState _syncState{ SyncState::PendingAdd };

        Wt::Dbo::ptr<User> _user;
        Wt::Dbo::ptr<Track> _track;
    };

} // namespace lms::db
