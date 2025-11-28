#pragma once

#include <functional>
#include <string>
#include <string_view>

#include <Wt/Dbo/Field.h>

#include "database/Object.hpp"
#include "database/Types.hpp"
#include "database/objects/TrackEmbeddedImageId.hpp"
#include "database/objects/TrackEmbeddedImageLinkId.hpp"
#include "database/objects/TrackId.hpp"

namespace lms::db
{
    class Session;
    class Track;
    class TrackEmbeddedImage;

    /**
     * @brief Track 嵌入式封面与 Track 之间的关联
     */
    class TrackEmbeddedImageLink final : public Object<TrackEmbeddedImageLink, TrackEmbeddedImageLinkId>
    {
    public:
        TrackEmbeddedImageLink() = default;

        static std::size_t getCount(Session& session);
        static pointer find(Session& session, TrackEmbeddedImageLinkId id);
        static void find(Session& session, TrackEmbeddedImageId trackEmbeddedImageId, const std::function<void(const pointer&)>& visitor);
        static void removeForTrack(Session& session, TrackId trackId);
        static pointer replace(Session& session,
                               const ObjectPtr<Track>& track,
                               const ObjectPtr<TrackEmbeddedImage>& image,
                               std::size_t index,
                               ImageType type,
                               std::string_view description);

        ObjectPtr<Track> getTrack() const;
        ObjectPtr<TrackEmbeddedImage> getImage() const;
        std::size_t getIndex() const { return static_cast<std::size_t>(_index); }
        ImageType getType() const { return _type; }
        std::string_view getDescription() const { return _description; }

        void setIndex(std::size_t index) { _index = static_cast<int>(index); }
        void setType(ImageType type) { _type = type; }
        void setDescription(std::string_view description) { _description.assign(description.begin(), description.end()); }

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
        friend class Session;
        static pointer create(Session& session, const ObjectPtr<Track>& track, const ObjectPtr<TrackEmbeddedImage>& image);

        int _index{};
        ImageType _type{ ImageType::Unknown };
        std::string _description;

        Wt::Dbo::ptr<Track> _track;
        Wt::Dbo::ptr<TrackEmbeddedImage> _image;
    };
} // namespace lms::db


