// 艺人评分实体定义

#pragma once

#include <optional>

#include <Wt/Dbo/Field.h>
#include <Wt/WDateTime.h>

#include "database/Object.hpp"
#include "database/Types.hpp"
#include "database/objects/ArtistId.hpp"
#include "database/objects/RatedArtistId.hpp"
#include "database/objects/UserId.hpp"

namespace lms::db
{
    class Artist;
    class Session;
    class User;

    class RatedArtist final : public Object<RatedArtist, RatedArtistId>
    {
    public:
        RatedArtist() = default;

        struct FindParameters
        {
            UserId user; // and this user
            std::optional<Range> range;

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
        static pointer find(Session& session, RatedArtistId id);
        static pointer find(Session& session, ArtistId artistId, UserId userId);
        static void find(Session& session, const FindParameters& findParams, std::function<void(const pointer&)> func);

        // Accessors
        ObjectPtr<Artist> getArtist() const { return _artist; }
        ObjectPtr<User> getUser() const { return _user; }
        Rating getRating() const { return _rating; }
        const Wt::WDateTime& getLastUpdated() const { return _lastUpdated; }

        // Setters
        void setRating(Rating rating) { _rating = rating; }
        void setLastUpdated(const Wt::WDateTime& lastUpdated);

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _rating, "rating");
            Wt::Dbo::field(a, _lastUpdated, "last_updated");

            Wt::Dbo::belongsTo(a, _artist, "artist", Wt::Dbo::OnDeleteCascade);
            Wt::Dbo::belongsTo(a, _user, "user", Wt::Dbo::OnDeleteCascade);
        }

    private:
        friend class Session;
        RatedArtist(ObjectPtr<Artist> artist, ObjectPtr<User> user);
        static pointer create(Session& session, ObjectPtr<Artist> artist, ObjectPtr<User> user);

        Rating _rating{};
        Wt::WDateTime _lastUpdated; // when it was rated for the last time

        Wt::Dbo::ptr<Artist> _artist;
        Wt::Dbo::ptr<User> _user;
    };
} // namespace lms::db
