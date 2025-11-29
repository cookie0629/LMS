#pragma once

#include <optional>
#include <string>
#include <string_view>

#include <Wt/Dbo/Field.h>

#include "database/Object.hpp"
#include "database/Types.hpp"
#include "database/objects/ArtistId.hpp"
#include "database/objects/TrackArtistLinkId.hpp"
#include "database/objects/TrackId.hpp"

namespace lms::db
{
    class Artist;
    class Session;
    class Track;

    /**
     * @brief Track 和 Artist 之间的关联（简化版）
     */
    class TrackArtistLink final : public Object<TrackArtistLink, TrackArtistLinkId>
    {
    public:
        TrackArtistLink() = default;

        // find
        static std::size_t getCount(Session& session);
        static pointer find(Session& session, TrackArtistLinkId id);
        static void find(Session& session, TrackId trackId, const std::function<void(const pointer&)>& func);
        static void removeForTrack(Session& session, TrackId trackId);

        // create
        static pointer create(Session& session,
                             const ObjectPtr<Track>& track,
                             const ObjectPtr<Artist>& artist,
                             TrackArtistLinkType type,
                             std::string_view subType = {},
                             bool artistMBIDMatched = false);

        // accessors
        ObjectPtr<Track> getTrack() const;
        ObjectPtr<Artist> getArtist() const;
        TrackArtistLinkType getType() const { return _type; }
        std::string_view getSubType() const { return _subType; }
        std::string_view getArtistName() const { return _artistName; }
        std::string_view getArtistSortName() const { return _artistSortName; }
        bool isArtistMBIDMatched() const { return _artistMBIDMatched; }

        // setters
        void setArtist(ObjectPtr<Artist> artist);
        void setArtistName(std::string_view artistName);
        void setArtistSortName(std::string_view artistSortName);

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _type, "type");
            Wt::Dbo::field(a, _subType, "subtype");
            Wt::Dbo::field(a, _artistName, "artist_name");
            Wt::Dbo::field(a, _artistSortName, "artist_sort_name");
            Wt::Dbo::field(a, _artistMBIDMatched, "artist_mbid_matched");

            Wt::Dbo::belongsTo(a, _track, "track", Wt::Dbo::OnDeleteCascade);
            Wt::Dbo::belongsTo(a, _artist, "artist", Wt::Dbo::OnDeleteCascade);
        }

    private:
        friend class Session;

        TrackArtistLinkType _type{ TrackArtistLinkType::Artist };
        std::string _subType;
        std::string _artistName;     // 标签中的艺术家名称
        std::string _artistSortName; // 标签中的艺术家排序名称
        bool _artistMBIDMatched{};

        Wt::Dbo::ptr<Track> _track;
        Wt::Dbo::ptr<Artist> _artist;
    };
} // namespace lms::db

