#pragma once

#include <filesystem>
#include <variant>

#include <Wt/Dbo/Field.h>
#include <Wt/WDateTime.h>

#include "database/Object.hpp"
#include "database/objects/ArtworkId.hpp"
#include "database/objects/ImageId.hpp"
#include "database/objects/TrackEmbeddedImageId.hpp"

namespace lms::db
{
    class Image;
    class Session;
    class TrackEmbeddedImage;

    /**
     * @brief Artwork 对象（支持 TrackEmbeddedImage 和 Image）
     */
    class Artwork final : public Object<Artwork, ArtworkId>
    {
    public:
        Artwork() = default;

        // find
        static std::size_t getCount(Session& session);
        static pointer find(Session& session, ArtworkId id);
        static pointer find(Session& session, TrackEmbeddedImageId id);
        static pointer find(Session& session, ImageId id);

        // getters
        using UnderlyingId = std::variant<std::monostate, TrackEmbeddedImageId, ImageId>;
        UnderlyingId getUnderlyingId() const;
        Wt::WDateTime getLastWrittenTime() const;
        std::filesystem::path getAbsoluteFilePath() const;
        ObjectPtr<TrackEmbeddedImage> getTrackEmbeddedImage() const;
        ObjectPtr<Image> getImage() const;
        TrackEmbeddedImageId getTrackEmbeddedImageId() const;
        ImageId getImageId() const;

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::belongsTo(a, _trackEmbeddedImage, "track_embedded_image", Wt::Dbo::OnDeleteCascade);
            Wt::Dbo::belongsTo(a, _image, "image", Wt::Dbo::OnDeleteCascade);
        }

    private:
        friend class Session;
        static pointer create(Session& session, ObjectPtr<TrackEmbeddedImage> trackEmbeddedImage);
        static pointer create(Session& session, ObjectPtr<Image> image);

        Wt::Dbo::ptr<TrackEmbeddedImage> _trackEmbeddedImage;
        Wt::Dbo::ptr<Image> _image;
    };
} // namespace lms::db

