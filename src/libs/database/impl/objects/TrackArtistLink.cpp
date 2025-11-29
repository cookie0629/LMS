#include "database/objects/TrackArtistLink.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/Artist.hpp"
#include "database/objects/Track.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::TrackArtistLink)

namespace lms::db
{
    std::size_t TrackArtistLink::getCount(Session& session)
    {
        session.checkReadTransaction();
        return session.getDboSession()->find<TrackArtistLink>().resultList().size();
    }

    TrackArtistLink::pointer TrackArtistLink::find(Session& session, TrackArtistLinkId id)
    {
        session.checkReadTransaction();
        auto result = session.getDboSession()
                          ->find<TrackArtistLink>()
                          .where("id = ?")
                          .bind(id.getValue())
                          .resultValue();
        if (!result)
        {
            return {};
        }
        return pointer(result);
    }

    void TrackArtistLink::find(Session& session, TrackId trackId, const std::function<void(const pointer&)>& func)
    {
        session.checkReadTransaction();
        auto results = session.getDboSession()
                           ->find<TrackArtistLink>()
                           .where("track_id = ?")
                           .bind(trackId.getValue())
                           .resultList();
        for (auto& link : results)
        {
            func(pointer(link));
        }
    }

    void TrackArtistLink::removeForTrack(Session& session, TrackId trackId)
    {
        session.checkWriteTransaction();
        auto results = session.getDboSession()
                           ->find<TrackArtistLink>()
                           .where("track_id = ?")
                           .bind(trackId.getValue())
                           .resultList();
        for (auto& link : results)
        {
            link.remove();
        }
    }

    TrackArtistLink::pointer TrackArtistLink::create(Session& session,
                                                     const ObjectPtr<Track>& track,
                                                     const ObjectPtr<Artist>& artist,
                                                     TrackArtistLinkType type,
                                                     std::string_view subType,
                                                     bool artistMBIDMatched)
    {
        session.checkWriteTransaction();

        // 检查是否已存在相同的关联
        auto existing = session.getDboSession()
                            ->find<TrackArtistLink>()
                            .where("track_id = ? AND artist_id = ? AND type = ?")
                            .bind(track->getId().getValue())
                            .bind(artist->getId().getValue())
                            .bind(static_cast<int>(type))
                            .limit(1)
                            .resultValue();

        if (existing)
        {
            return pointer(existing);
        }

        auto dboPtr = session.getDboSession()->add(std::make_unique<TrackArtistLink>());
        auto link = dboPtr.modify();
        link->_track = getDboPtr(track);
        link->_artist = getDboPtr(artist);
        link->_type = type;
        link->_subType.assign(subType.begin(), subType.end());
        link->_artistName = artist->getName();
        if (auto sortName = artist->getSortName(); !sortName.empty())
        {
            link->_artistSortName = sortName;
        }
        link->_artistMBIDMatched = artistMBIDMatched;

        return pointer(dboPtr);
    }

    ObjectPtr<Track> TrackArtistLink::getTrack() const
    {
        if (!_track)
        {
            return {};
        }
        return ObjectPtr<Track>(_track);
    }

    ObjectPtr<Artist> TrackArtistLink::getArtist() const
    {
        if (!_artist)
        {
            return {};
        }
        return ObjectPtr<Artist>(_artist);
    }

    void TrackArtistLink::setArtist(ObjectPtr<Artist> artist)
    {
        _artist = getDboPtr(artist);
        if (artist)
        {
            _artistName = artist->getName();
            if (auto sortName = artist->getSortName(); !sortName.empty())
            {
                _artistSortName = sortName;
            }
        }
    }

    void TrackArtistLink::setArtistName(std::string_view artistName)
    {
        _artistName.assign(artistName.begin(), artistName.end());
    }

    void TrackArtistLink::setArtistSortName(std::string_view artistSortName)
    {
        _artistSortName.assign(artistSortName.begin(), artistSortName.end());
    }
} // namespace lms::db

