
#pragma once

#include <string>
#include <string_view>

#include <Wt/Dbo/Field.h>

#include "database/Object.hpp"
#include "database/Types.hpp"
#include "database/objects/TrackEmbeddedImageId.hpp"
#include "database/objects/TrackEmbeddedImageLinkId.hpp"

namespace lms::db
{
    class TrackEmbeddedImage;
    class Session;
    class Track;

    class TrackEmbeddedImageLink final : public Object<TrackEmbeddedImageLink, TrackEmbeddedImageLinkId>
    {
    public:
        TrackEmbeddedImageLink() = default;

        // find
        static std::size_t getCount(Session& session);
        static pointer find(Session& session, TrackEmbeddedImageLinkId id);
        static void find(Session& session, TrackEmbeddedImageId trackEmbeddedImageId, std::function<void(const pointer&)> visitor);

        // getters
        ObjectPtr<Track> getTrack() const;
        ObjectPtr<TrackEmbeddedImage> getImage() const;
        std::size_t getIndex() const { return _index; }
        ImageType getType() const { return _type; }
        std::string_view getDescription() const { return _description; }

        // setters
        void setIndex(std::size_t index) { _index = static_cast<int>(index); }
        void setType(ImageType type) { _type = type; }
        void setDescription(std::string_view description) { _description = description; }

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _index, "index");
            Wt::Dbo::field(a, _type, "type");
            Wt::Dbo::field(a, _description, "description");

            Wt::Dbo::belongsTo(a, _track, "track", Wt::Dbo::OnDeleteCascade);
            Wt::Dbo::belongsTo(a, _image, "track_embedded_image", Wt::Dbo::OnDeleteCascade);
        }

    private:
        TrackEmbeddedImageLink(ObjectPtr<Track> track, ObjectPtr<TrackEmbeddedImage> image);

        friend class Session;
        static pointer create(Session& session, ObjectPtr<Track> track, ObjectPtr<TrackEmbeddedImage> image);

        int _index{}; // index within the track
        ImageType _type{ ImageType::Unknown };
        std::string _description;

        Wt::Dbo::ptr<Track> _track;
        Wt::Dbo::ptr<TrackEmbeddedImage> _image;
    };
} // namespace lms::db
