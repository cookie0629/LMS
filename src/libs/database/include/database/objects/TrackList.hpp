#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <Wt/Dbo/Field.h>
#include <Wt/Dbo/collection.h>
#include <Wt/WDateTime.h>

#include "database/Object.hpp"
#include "database/Types.hpp"
#include "database/objects/TrackId.hpp"
#include "database/objects/TrackListId.hpp"
#include "database/objects/UserId.hpp"

namespace lms::db
{
    class Session;
    class Track;
    class TrackListEntry;
    class User;

    /**
     * @brief 播放列表对象（简化版）
     */
    class TrackList final : public Object<TrackList, TrackListId>
    {
    public:
        enum class Visibility
        {
            Private = 0,
            Public = 1,
        };

        TrackList() = default;

        // find
        static std::size_t getCount(Session& session);
        static pointer find(Session& session, TrackListId id);
        static pointer find(Session& session, std::string_view name, TrackListType type, UserId userId);

        // accessors
        std::string_view getName() const { return _name; }
        Visibility getVisibility() const { return _visibility; }
        TrackListType getType() const { return _type; }
        ObjectPtr<User> getUser() const;
        UserId getUserId() const;
        Wt::WDateTime getLastModifiedDateTime() const { return _lastModifiedDateTime; }
        Wt::WDateTime getCreationDateTime() const { return _creationDateTime; }

        // modifiers
        void setUser(ObjectPtr<User> user);
        void setName(std::string_view name) { _name = name; }
        void setVisibility(Visibility visibility) { _visibility = visibility; }
        void setLastModifiedDateTime(const Wt::WDateTime& dateTime) { _lastModifiedDateTime = dateTime; }

        // entries
        bool isEmpty() const;
        std::size_t getCount() const;
        std::vector<TrackId> getTrackIds() const;

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _name, "name");
            Wt::Dbo::field(a, _type, "type");
            Wt::Dbo::field(a, _visibility, "visibility");
            Wt::Dbo::field(a, _creationDateTime, "creation_date_time");
            Wt::Dbo::field(a, _lastModifiedDateTime, "last_modified_date_time");

            Wt::Dbo::belongsTo(a, _user, "user", Wt::Dbo::OnDeleteCascade);
            Wt::Dbo::hasMany(a, _entries, Wt::Dbo::ManyToOne, "tracklist");
        }

    private:
        friend class Session;
        TrackList(std::string_view name, TrackListType type);
        static pointer create(Session& session, std::string_view name, TrackListType type);

        std::string _name;
        TrackListType _type{ TrackListType::PlayList };
        Visibility _visibility{ Visibility::Private };
        Wt::WDateTime _creationDateTime;
        Wt::WDateTime _lastModifiedDateTime;

        Wt::Dbo::ptr<User> _user;
        Wt::Dbo::collection<Wt::Dbo::ptr<TrackListEntry>> _entries;
    };

    /**
     * @brief 播放列表条目对象（简化版）
     */
    class TrackListEntry final : public Object<TrackListEntry, TrackListEntryId>
    {
    public:
        TrackListEntry() = default;

        // find
        static pointer find(Session& session, TrackListEntryId id);
        static void find(Session& session, TrackListId trackListId, const std::function<void(const pointer&)>& func);

        // accessors
        TrackId getTrackId() const;
        ObjectPtr<Track> getTrack() const;
        ObjectPtr<TrackList> getTrackList() const;
        const Wt::WDateTime& getDateTime() const { return _dateTime; }

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _dateTime, "date_time");

            Wt::Dbo::belongsTo(a, _track, "track", Wt::Dbo::OnDeleteCascade);
            Wt::Dbo::belongsTo(a, _tracklist, "tracklist", Wt::Dbo::OnDeleteCascade);
        }

    private:
        friend class Session;
        TrackListEntry(ObjectPtr<Track> track, ObjectPtr<TrackList> tracklist, const Wt::WDateTime& dateTime = {});
        static pointer create(Session& session, ObjectPtr<Track> track, ObjectPtr<TrackList> tracklist, const Wt::WDateTime& dateTime = {});

        Wt::WDateTime _dateTime;
        Wt::Dbo::ptr<Track> _track;
        Wt::Dbo::ptr<TrackList> _tracklist;
    };
} // namespace lms::db

