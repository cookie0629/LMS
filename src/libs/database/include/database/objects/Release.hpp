#pragma once

#include <string>
#include <string_view>

#include <Wt/Dbo/Field.h>
#include <Wt/Dbo/collection.h>
#include <Wt/WDateTime.h>

#include "database/objects/ReleaseId.hpp"
#include "database/Object.hpp"

namespace lms::db
{
    class Artist;
    class Medium;
    class Session;

    /**
     * @brief 专辑/发行对象（简化版）
     */
    class Release final : public Object<Release, ReleaseId>
    {
    public:
        static constexpr std::size_t maxNameLength{ 512 };

        Release() = default;

        // 静态方法
        static std::size_t getCount(Session& session);
        static pointer find(Session& session, ReleaseId id);
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
        Release(std::string_view name);
        static pointer create(Session& session, std::string_view name);

        std::string _name;
        std::string _sortName;
    };
} // namespace lms::db

