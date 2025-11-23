#pragma once

#include <optional>
#include <string>
#include <string_view>

#include <Wt/Dbo/Field.h>
#include <Wt/Dbo/collection.h>

#include "database/Object.hpp"
#include "database/objects/MediumId.hpp"
#include "database/objects/ReleaseId.hpp"

namespace lms::db
{
    class Release;
    class Session;
    class Track;

    /**
     * @brief 介质对象（简化版）
     * 表示专辑中的一个光盘/介质
     */
    class Medium final : public Object<Medium, MediumId>
    {
    public:
        static constexpr std::size_t maxNameLength{ 256 };

        Medium() = default;

        // 静态方法
        static std::size_t getCount(Session& session);
        static pointer find(Session& session, MediumId id);
        static pointer find(Session& session, ReleaseId releaseId, std::optional<int> position);
        static pointer getOrCreate(Session& session, ReleaseId releaseId, std::optional<int> position = std::nullopt);

        // 访问器
        std::string_view getName() const { return _name; }
        void setName(std::string_view name);

        std::optional<int> getPosition() const { return _position; }
        void setPosition(std::optional<int> position) { _position = position; }

        ReleaseId getReleaseId() const;
        void setRelease(ObjectPtr<Release> release);

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _name, "name");
            Wt::Dbo::field(a, _position, "position");
            Wt::Dbo::belongsTo(a, _release, "release_id");
        }

    private:
        friend class Session;
        Medium(std::string_view name, ObjectPtr<Release> release, std::optional<int> position);
        static pointer create(Session& session, std::string_view name, ObjectPtr<Release> release, std::optional<int> position);

        std::string _name;
        std::optional<int> _position;
        Wt::Dbo::ptr<Release> _release;
    };
} // namespace lms::db

