#include "database/objects/Artwork.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/TrackEmbeddedImage.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::Artwork)

namespace lms::db
{
    std::size_t Artwork::getCount(Session& session)
    {
        session.checkReadTransaction();
        return session.getDboSession()->find<Artwork>().resultList().size();
    }

    Artwork::pointer Artwork::find(Session& session, ArtworkId id)
    {
        session.checkReadTransaction();
        auto result = session.getDboSession()
                          ->find<Artwork>()
                          .where("id = ?")
                          .bind(id.getValue())
                          .resultValue();
        if (!result)
        {
            return {};
        }
        return pointer(result);
    }

    Artwork::pointer Artwork::find(Session& session, TrackEmbeddedImageId id)
    {
        session.checkReadTransaction();
        auto result = session.getDboSession()
                          ->find<Artwork>()
                          .where("track_embedded_image_id = ?")
                          .bind(id.getValue())
                          .limit(1)
                          .resultValue();
        if (!result)
        {
            return {};
        }
        return pointer(result);
    }

    Artwork::UnderlyingId Artwork::getUnderlyingId() const
    {
        if (_trackEmbeddedImage)
        {
            return TrackEmbeddedImageId{ _trackEmbeddedImage.id() };
        }
        return std::monostate{};
    }

    Wt::WDateTime Artwork::getLastWrittenTime() const
    {
        if (_trackEmbeddedImage)
        {
            return _trackEmbeddedImage->getUpdatedAt();
        }
        return Wt::WDateTime{};
    }

    ObjectPtr<TrackEmbeddedImage> Artwork::getTrackEmbeddedImage() const
    {
        if (!_trackEmbeddedImage)
        {
            return {};
        }
        return ObjectPtr<TrackEmbeddedImage>(_trackEmbeddedImage);
    }

    TrackEmbeddedImageId Artwork::getTrackEmbeddedImageId() const
    {
        if (!_trackEmbeddedImage)
        {
            return TrackEmbeddedImageId{};
        }
        return TrackEmbeddedImageId{ _trackEmbeddedImage.id() };
    }

    Artwork::pointer Artwork::create(Session& session, ObjectPtr<TrackEmbeddedImage> trackEmbeddedImage)
    {
        session.checkWriteTransaction();
        auto artwork = std::make_unique<Artwork>();
        artwork->_trackEmbeddedImage = getDboPtr(trackEmbeddedImage);
        auto dboPtr = session.getDboSession()->add(std::move(artwork));
        return pointer(dboPtr);
    }
} // namespace lms::db

