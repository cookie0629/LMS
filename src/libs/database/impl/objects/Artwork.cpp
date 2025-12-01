#include "database/objects/Artwork.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/Image.hpp"
#include "database/objects/TrackEmbeddedImage.hpp"
#include "traits/PathTraits.hpp"

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

    Artwork::pointer Artwork::find(Session& session, ImageId id)
    {
        session.checkReadTransaction();
        auto result = session.getDboSession()
                          ->find<Artwork>()
                          .where("image_id = ?")
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
        if (_image)
        {
            return ImageId{ _image.id() };
        }
        return std::monostate{};
    }

    Wt::WDateTime Artwork::getLastWrittenTime() const
    {
        if (_trackEmbeddedImage)
        {
            return _trackEmbeddedImage->getUpdatedAt();
        }
        if (_image)
        {
            return _image->getLastWriteTime();
        }
        return Wt::WDateTime{};
    }

    std::filesystem::path Artwork::getAbsoluteFilePath() const
    {
        if (_image)
        {
            return _image->getAbsoluteFilePath();
        }
        return {};
    }

    ObjectPtr<TrackEmbeddedImage> Artwork::getTrackEmbeddedImage() const
    {
        if (!_trackEmbeddedImage)
        {
            return {};
        }
        return ObjectPtr<TrackEmbeddedImage>(_trackEmbeddedImage);
    }

    ObjectPtr<Image> Artwork::getImage() const
    {
        if (!_image)
        {
            return {};
        }
        return ObjectPtr<Image>(_image);
    }

    TrackEmbeddedImageId Artwork::getTrackEmbeddedImageId() const
    {
        if (!_trackEmbeddedImage)
        {
            return TrackEmbeddedImageId{};
        }
        return TrackEmbeddedImageId{ _trackEmbeddedImage.id() };
    }

    ImageId Artwork::getImageId() const
    {
        if (!_image)
        {
            return ImageId{};
        }
        return ImageId{ _image.id() };
    }

    Artwork::pointer Artwork::create(Session& session, ObjectPtr<TrackEmbeddedImage> trackEmbeddedImage)
    {
        session.checkWriteTransaction();
        auto artwork = std::make_unique<Artwork>();
        artwork->_trackEmbeddedImage = getDboPtr(trackEmbeddedImage);
        auto dboPtr = session.getDboSession()->add(std::move(artwork));
        return pointer(dboPtr);
    }

    Artwork::pointer Artwork::create(Session& session, ObjectPtr<Image> image)
    {
        session.checkWriteTransaction();
        auto artwork = std::make_unique<Artwork>();
        artwork->_image = getDboPtr(image);
        auto dboPtr = session.getDboSession()->add(std::move(artwork));
        return pointer(dboPtr);
    }
} // namespace lms::db

