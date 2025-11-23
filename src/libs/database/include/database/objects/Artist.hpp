#pragma once

#include <string>
#include <string_view>

#include <Wt/Dbo/Field.h>
#include <Wt/Dbo/collection.h>

#include "database/objects/ArtistId.hpp"
#include "database/Object.hpp"

namespace lms::db
{
    class Track;
    class Session;

    /**
     * @brief 艺术家对象（简化版）
     */
    class Artist final : public Object<Artist, ArtistId>
    {
    public:
        static constexpr std::size_t maxNameLength{ 512 };

        Artist() = default;

        // 静态方法
        static std::size_t getCount(Session& session);
        static pointer find(Session& session, ArtistId id);
        static pointer find(Session& session, std::string_view name);
        static pointer getOrCreate(Session& session, std::string_view name);

        // 访问器
        std::string_view getName() const { return _name; }
        void setName(std::string_view name);

        std::string_view getSortName() const { return _sortName; }
        void setSortName(std::string_view sortName);

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _name, "name");
            Wt::Dbo::field(a, _sortName, "sort_name");
        }

    private:
        friend class Session;
        Artist(std::string_view name);
        static pointer create(Session& session, std::string_view name);

        std::string _name;
        std::string _sortName;
    };
} // namespace lms::db

