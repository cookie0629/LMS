
#include "database/objects/TrackEmbeddedImageLink.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "database/Session.hpp"
#include "database/Types.hpp"
#include "database/objects/Track.hpp"
#include "database/objects/TrackEmbeddedImage.hpp"

#include "Utils.hpp"
#include "traits/IdTypeTraits.hpp"
#include "traits/ImageHashTypeTraits.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::TrackEmbeddedImageLink)

namespace lms::db
{
    TrackEmbeddedImageLink::TrackEmbeddedImageLink(ObjectPtr<Track> track, ObjectPtr<TrackEmbeddedImage> image)
        : _track{ getDboPtr(track) }
        , _image{ getDboPtr(image) }
    {
    }

    TrackEmbeddedImageLink::pointer TrackEmbeddedImageLink::create(Session& session, ObjectPtr<Track> track, ObjectPtr<TrackEmbeddedImage> image)
    {
        return session.getDboSession()->add(std::unique_ptr<TrackEmbeddedImageLink>{ new TrackEmbeddedImageLink{ track, image } });
    }

    std::size_t TrackEmbeddedImageLink::getCount(Session& session)
    {
        session.checkReadTransaction();

        return utils::fetchQuerySingleResult(session.getDboSession()->query<int>("SELECT COUNT(*) FROM track_embedded_image_link"));
    }

    TrackEmbeddedImageLink::pointer TrackEmbeddedImageLink::find(Session& session, TrackEmbeddedImageLinkId id)
    {
        session.checkReadTransaction();

        return utils::fetchQuerySingleResult(session.getDboSession()->find<TrackEmbeddedImageLink>().where("id = ?").bind(id));
    }

    void TrackEmbeddedImageLink::find(Session& session, TrackEmbeddedImageId trackEmbeddedImageId, std::function<void(const pointer&)> visitor)
    {
        auto query{ session.getDboSession()->find<TrackEmbeddedImageLink>() };
        query.where("track_embedded_image_id = ?").bind(trackEmbeddedImageId);

        return utils::forEachQueryResult(query, visitor);
    }

    ObjectPtr<Track> TrackEmbeddedImageLink::getTrack() const
    {
        return _track;
    }

    ObjectPtr<TrackEmbeddedImage> TrackEmbeddedImageLink::getImage() const
    {
        return _image;
    }
} // namespace lms::db
