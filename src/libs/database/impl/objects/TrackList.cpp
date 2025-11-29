#include "database/objects/TrackList.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/Track.hpp"
#include "database/objects/User.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::TrackList)
DBO_INSTANTIATE_TEMPLATES(lms::db::TrackListEntry)

namespace lms::db
{
    // TrackList implementation
    std::size_t TrackList::getCount(Session& session)
    {
        session.checkReadTransaction();
        return session.getDboSession()->find<TrackList>().resultList().size();
    }

    TrackList::pointer TrackList::find(Session& session, TrackListId id)
    {
        session.checkReadTransaction();
        auto result = session.getDboSession()
                          ->find<TrackList>()
                          .where("id = ?")
                          .bind(id.getValue())
                          .resultValue();
        if (!result)
        {
            return {};
        }
        return pointer(result);
    }

    TrackList::pointer TrackList::find(Session& session, std::string_view name, TrackListType type, UserId userId)
    {
        session.checkReadTransaction();
        auto result = session.getDboSession()
                          ->find<TrackList>()
                          .where("name = ? AND type = ? AND user_id = ?")
                          .bind(std::string{ name })
                          .bind(static_cast<int>(type))
                          .bind(userId.getValue())
                          .limit(1)
                          .resultValue();
        if (!result)
        {
            return {};
        }
        return pointer(result);
    }

    ObjectPtr<User> TrackList::getUser() const
    {
        if (!_user)
        {
            return {};
        }
        return ObjectPtr<User>(_user);
    }

    UserId TrackList::getUserId() const
    {
        if (!_user)
        {
            return UserId{};
        }
        return UserId{ _user.id() };
    }

    void TrackList::setUser(ObjectPtr<User> user)
    {
        _user = getDboPtr(user);
    }

    bool TrackList::isEmpty() const
    {
        return _entries.size() == 0;
    }

    std::size_t TrackList::getCount() const
    {
        return _entries.size();
    }

    std::vector<TrackId> TrackList::getTrackIds() const
    {
        std::vector<TrackId> trackIds;
        for (const auto& entry : _entries)
        {
            if (entry)
            {
                trackIds.push_back(TrackId{ entry->getTrackId() });
            }
        }
        return trackIds;
    }

    TrackList::TrackList(std::string_view name, TrackListType type)
        : _name{ name }
        , _type{ type }
        , _creationDateTime{ Wt::WDateTime::currentDateTime() }
        , _lastModifiedDateTime{ Wt::WDateTime::currentDateTime() }
    {
    }

    TrackList::pointer TrackList::create(Session& session, std::string_view name, TrackListType type)
    {
        session.checkWriteTransaction();
        auto trackList = std::unique_ptr<TrackList>(new TrackList(name, type));
        auto dboPtr = session.getDboSession()->add(std::move(trackList));
        return pointer(dboPtr);
    }

    // TrackListEntry implementation
    TrackListEntry::pointer TrackListEntry::find(Session& session, TrackListEntryId id)
    {
        session.checkReadTransaction();
        auto result = session.getDboSession()
                          ->find<TrackListEntry>()
                          .where("id = ?")
                          .bind(id.getValue())
                          .resultValue();
        if (!result)
        {
            return {};
        }
        return pointer(result);
    }

    void TrackListEntry::find(Session& session, TrackListId trackListId, const std::function<void(const pointer&)>& func)
    {
        session.checkReadTransaction();
        auto results = session.getDboSession()
                           ->find<TrackListEntry>()
                           .where("tracklist_id = ?")
                           .bind(trackListId.getValue())
                           .resultList();
        for (auto& entry : results)
        {
            func(pointer(entry));
        }
    }

    TrackId TrackListEntry::getTrackId() const
    {
        if (!_track)
        {
            return TrackId{};
        }
        return TrackId{ _track.id() };
    }

    ObjectPtr<Track> TrackListEntry::getTrack() const
    {
        if (!_track)
        {
            return {};
        }
        return ObjectPtr<Track>(_track);
    }

    ObjectPtr<TrackList> TrackListEntry::getTrackList() const
    {
        if (!_tracklist)
        {
            return {};
        }
        return ObjectPtr<TrackList>(_tracklist);
    }

    TrackListEntry::TrackListEntry(ObjectPtr<Track> track, ObjectPtr<TrackList> tracklist, const Wt::WDateTime& dateTime)
        : _dateTime{ dateTime.isValid() ? dateTime : Wt::WDateTime::currentDateTime() }
        , _track{ getDboPtr(track) }
        , _tracklist{ getDboPtr(tracklist) }
    {
    }

    TrackListEntry::pointer TrackListEntry::create(Session& session, ObjectPtr<Track> track, ObjectPtr<TrackList> tracklist, const Wt::WDateTime& dateTime)
    {
        session.checkWriteTransaction();
        auto entry = std::unique_ptr<TrackListEntry>(new TrackListEntry(track, tracklist, dateTime));
        auto dboPtr = session.getDboSession()->add(std::move(entry));
        return pointer(dboPtr);
    }
} // namespace lms::db

