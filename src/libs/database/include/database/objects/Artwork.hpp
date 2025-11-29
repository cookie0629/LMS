#pragma once

#include <filesystem>
#include <variant>

#include <Wt/Dbo/Field.h>
#include <Wt/WDateTime.h>

#include "database/Object.hpp"
#include "database/objects/ArtworkId.hpp"
#include "database/objects/TrackEmbeddedImageId.hpp"

namespace lms::db
{
    class Session;
    class TrackEmbeddedImage;

    /**
     * @brief Artwork 对象（简化版，只支持 TrackEmbeddedImage）
     */
    class Artwork final : public Object<Artwork, ArtworkId>
    {
    public:
        Artwork() = default;

        // find
        static std::size_t getCount(Session& session);
        static pointer find(Session& session, ArtworkId id);
        static pointer find(Session& session, TrackEmbeddedImageId id);

        // getters
        using UnderlyingId = std::variant<std::monostate, TrackEmbeddedImageId>;
        UnderlyingId getUnderlyingId() const;
        Wt::WDateTime getLastWrittenTime() const;
        ObjectPtr<TrackEmbeddedImage> getTrackEmbeddedImage() const;
        TrackEmbeddedImageId getTrackEmbeddedImageId() const;

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::belongsTo(a, _trackEmbeddedImage, "track_embedded_image", Wt::Dbo::OnDeleteCascade);
        }

    private:
        friend class Session;
        static pointer create(Session& session, ObjectPtr<TrackEmbeddedImage> trackEmbeddedImage);

        Wt::Dbo::ptr<TrackEmbeddedImage> _trackEmbeddedImage;
    };
} // namespace lms::db

