#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <string_view>

#include <Wt/Dbo/Field.h>
#include <Wt/WDateTime.h>

#include "database/Object.hpp"
#include "database/Types.hpp"
#include "database/objects/TrackEmbeddedImageId.hpp"
#include "database/objects/TrackId.hpp"

namespace lms::db
{
    class Session;
    class Track;

    /**
     * @brief 存储 Track 的嵌入式封面数据（简化版）
     */
    class TrackEmbeddedImage final : public Object<TrackEmbeddedImage, TrackEmbeddedImageId>
    {
    public:
        TrackEmbeddedImage() = default;

        static pointer find(Session& session, TrackEmbeddedImageId id);
        static pointer findByTrack(Session& session, TrackId trackId);
        static void removeForTrack(Session& session, TrackId trackId);
        static pointer upsert(Session& session,
                              const ObjectPtr<Track>& track,
                              std::span<const std::byte> data,
                              std::string_view mimeType,
                              std::size_t width,
                              std::size_t height,
                              ImageType type = ImageType::Unknown,
                              std::string_view description = {},
                              std::size_t index = 0);

        const std::string& getData() const { return _data; }
        std::string_view getMimeType() const { return _mimeType; }
        std::size_t getWidth() const { return static_cast<std::size_t>(_width); }
        std::size_t getHeight() const { return static_cast<std::size_t>(_height); }
        const Wt::WDateTime& getUpdatedAt() const { return _updatedAt; }

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _data, "data");
            Wt::Dbo::field(a, _mimeType, "mime_type");
            Wt::Dbo::field(a, _width, "width");
            Wt::Dbo::field(a, _height, "height");
            Wt::Dbo::field(a, _updatedAt, "updated_at");
            Wt::Dbo::belongsTo(a, _track, "track", Wt::Dbo::OnDeleteCascade);
        }

    private:
        friend class Session;

        static pointer create(Session& session, const ObjectPtr<Track>& track);
        void setData(std::span<const std::byte> data);
        void setMimeType(std::string_view mimeType);
        void setSize(std::size_t width, std::size_t height);
        void touch();

        std::string _data;
        std::string _mimeType;
        int _width{};
        int _height{};
        Wt::WDateTime _updatedAt;
        Wt::Dbo::ptr<Track> _track;
    };
} // namespace lms::db


