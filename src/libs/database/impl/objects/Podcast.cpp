
#include "database/objects/Podcast.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "database/Session.hpp"
#include "database/objects/Artwork.hpp"
#include "database/objects/PodcastEpisode.hpp"

#include "Utils.hpp"
#include "traits/IdTypeTraits.hpp"
#include "traits/StringViewTraits.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::Podcast)

namespace lms::db
{
    Podcast::Podcast(std::string_view url)
        : _url{ url }
    {
    }

    Podcast::pointer Podcast::create(Session& session, std::string_view url)
    {
        return session.getDboSession()->add(std::unique_ptr<Podcast>{ new Podcast{ url } });
    }

    std::size_t Podcast::getCount(Session& session)
    {
        session.checkReadTransaction();
        return utils::fetchQuerySingleResult(session.getDboSession()->query<int>("SELECT COUNT(*) FROM podcast"));
    }

    Podcast::pointer Podcast::find(Session& session, PodcastId id)
    {
        session.checkReadTransaction();
        return utils::fetchQuerySingleResult(session.getDboSession()->query<Wt::Dbo::ptr<Podcast>>("SELECT p from podcast p").where("p.id = ?").bind(id));
    }

    Podcast::pointer Podcast::find(Session& session, std::string_view url)
    {
        session.checkReadTransaction();
        return utils::fetchQuerySingleResult(session.getDboSession()->query<Wt::Dbo::ptr<Podcast>>("SELECT p from podcast p").where("p.url = ?").bind(url));
    }

    void Podcast::find(Session& session, std::function<void(const pointer&)> func)
    {
        session.checkReadTransaction();

        auto query{ session.getDboSession()->query<Wt::Dbo::ptr<Podcast>>("SELECT p from podcast p") };
        utils::forEachQueryResult(query, func);
    }

    ObjectPtr<Artwork> Podcast::getArtwork() const
    {
        return _artwork;
    }

    ArtworkId Podcast::getArtworkId() const
    {
        return _artwork.id();
    }

    void Podcast::setArtwork(ObjectPtr<Artwork> artwork)
    {
        _artwork = getDboPtr(artwork);
    }
} // namespace lms::db
