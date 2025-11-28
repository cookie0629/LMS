#include "database/objects/TrackEmbeddedImage.hpp"

#include <cstring>
#include <memory>

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/Track.hpp"
#include "database/objects/TrackEmbeddedImageLink.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::TrackEmbeddedImage)

namespace lms::db
{
    namespace
    {
        constexpr std::size_t kDefaultImageDimension{ 0 };
    } // namespace

    TrackEmbeddedImage::pointer TrackEmbeddedImage::find(Session& session, TrackEmbeddedImageId id)
    {
        session.checkReadTransaction();
        auto result = session.getDboSession()
                          ->find<TrackEmbeddedImage>()
                          .where("id = ?")
                          .bind(id.getValue())
                          .resultValue();
        if (!result)
        {
            return {};
        }
        return pointer(result);
    }

    TrackEmbeddedImage::pointer TrackEmbeddedImage::findByTrack(Session& session, TrackId trackId)
    {
        session.checkReadTransaction();
        auto result = session.getDboSession()
                          ->find<TrackEmbeddedImage>()
                          .where("track_id = ?")
                          .bind(trackId.getValue())
                          .limit(1)
                          .resultValue();
        if (!result)
        {
            return {};
        }
        return pointer(result);
    }

    void TrackEmbeddedImage::removeForTrack(Session& session, TrackId trackId)
    {
        session.checkWriteTransaction();
        auto results = session.getDboSession()
                           ->find<TrackEmbeddedImage>()
                           .where("track_id = ?")
                           .bind(trackId.getValue())
                           .resultList();
        for (auto& image : results)
        {
            image.remove();
        }
    }

    TrackEmbeddedImage::pointer TrackEmbeddedImage::upsert(Session& session,
                                                           const ObjectPtr<Track>& track,
                                                           std::span<const std::byte> data,
                                                           std::string_view mimeType,
                                                           std::size_t width,
                                                           std::size_t height,
                                                           ImageType type,
                                                           std::string_view description,
                                                           std::size_t index)
    {
        session.checkWriteTransaction();
        auto existing = findByTrack(session, track->getId());
        if (!existing)
        {
            existing = create(session, track);
        }

        auto updater = existing.modify();
        updater->setData(data);
        updater->setMimeType(mimeType);
        updater->setSize(width, height);
        updater->touch();
        TrackEmbeddedImageLink::replace(session, track, existing, index, type, description);
        return existing;
    }

    TrackEmbeddedImage::pointer TrackEmbeddedImage::create(Session& session, const ObjectPtr<Track>& track)
    {
        auto dboPtr = session.getDboSession()->add(std::make_unique<TrackEmbeddedImage>());
        dboPtr.modify()->_track = getDboPtr(track);
        dboPtr.modify()->setSize(kDefaultImageDimension, kDefaultImageDimension);
        dboPtr.modify()->touch();
        return pointer(dboPtr);
    }

    void TrackEmbeddedImage::setData(std::span<const std::byte> data)
    {
        if (data.empty())
        {
            _data.clear();
            return;
        }

        _data.assign(reinterpret_cast<const char*>(data.data()), data.size());
    }

    void TrackEmbeddedImage::setMimeType(std::string_view mimeType)
    {
        _mimeType.assign(mimeType.begin(), mimeType.end());
    }

    void TrackEmbeddedImage::setSize(std::size_t width, std::size_t height)
    {
        _width = static_cast<int>(width);
        _height = static_cast<int>(height);
    }

    void TrackEmbeddedImage::touch()
    {
        _updatedAt = Wt::WDateTime::currentDateTime();
    }
} // namespace lms::db


