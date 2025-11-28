#include "database/objects/TrackEmbeddedImageLink.hpp"

#include <memory>

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/Track.hpp"
#include "database/objects/TrackEmbeddedImage.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::TrackEmbeddedImageLink)

namespace lms::db
{
    std::size_t TrackEmbeddedImageLink::getCount(Session& session)
    {
        session.checkReadTransaction();
        return session.getDboSession()->query<int>("select count(*) from track_embedded_image_link");
    }

    TrackEmbeddedImageLink::pointer TrackEmbeddedImageLink::find(Session& session, TrackEmbeddedImageLinkId id)
    {
        session.checkReadTransaction();
        auto result = session.getDboSession()
                          ->find<TrackEmbeddedImageLink>()
                          .where("id = ?")
                          .bind(id.getValue())
                          .resultValue();
        if (!result)
        {
            return {};
        }
        return pointer(result);
    }

    void TrackEmbeddedImageLink::find(Session& session, TrackEmbeddedImageId trackEmbeddedImageId, const std::function<void(const pointer&)>& visitor)
    {
        session.checkReadTransaction();
        auto results = session.getDboSession()
                           ->find<TrackEmbeddedImageLink>()
                           .where("track_embedded_image_id = ?")
                           .bind(trackEmbeddedImageId.getValue())
                           .resultList();
        for (auto& link : results)
        {
            visitor(pointer(link));
        }
    }

    void TrackEmbeddedImageLink::removeForTrack(Session& session, TrackId trackId)
    {
        session.checkWriteTransaction();
        auto results = session.getDboSession()
                           ->find<TrackEmbeddedImageLink>()
                           .where("track_id = ?")
                           .bind(trackId.getValue())
                           .resultList();
        for (auto& link : results)
        {
            link.remove();
        }
    }

    TrackEmbeddedImageLink::pointer TrackEmbeddedImageLink::replace(Session& session,
                                                                    const ObjectPtr<Track>& track,
                                                                    const ObjectPtr<TrackEmbeddedImage>& image,
                                                                    std::size_t index,
                                                                    ImageType type,
                                                                    std::string_view description)
    {
        session.checkWriteTransaction();
        removeForTrack(session, track->getId());
        auto link = create(session, track, image);
        auto modifier = link.modify();
        modifier->setIndex(index);
        modifier->setType(type);
        modifier->setDescription(description);
        return link;
    }

    ObjectPtr<Track> TrackEmbeddedImageLink::getTrack() const
    {
        return ObjectPtr<Track>(_track);
    }

    ObjectPtr<TrackEmbeddedImage> TrackEmbeddedImageLink::getImage() const
    {
        return ObjectPtr<TrackEmbeddedImage>(_image);
    }

    TrackEmbeddedImageLink::pointer TrackEmbeddedImageLink::create(Session& session, const ObjectPtr<Track>& track, const ObjectPtr<TrackEmbeddedImage>& image)
    {
        auto dboPtr = session.getDboSession()->add(std::make_unique<TrackEmbeddedImageLink>());
        dboPtr.modify()->_track = getDboPtr(track);
        dboPtr.modify()->_image = getDboPtr(image);
        return pointer(dboPtr);
    }
} // namespace lms::db


