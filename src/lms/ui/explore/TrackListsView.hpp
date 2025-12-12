
#pragma once

#include <unordered_map>

#include <Wt/WTemplate.h>

#include "database/Object.hpp"
#include "database/objects/TrackListId.hpp"

#include "common/Template.hpp"

namespace lms::db
{
    class TrackList;
}

namespace lms::ui
{
    class Filters;
    class InfiniteScrollingContainer;

    class TrackLists : public Template
    {
    public:
        TrackLists(Filters& filters);

        void onTrackListDeleted(db::TrackListId trackListId);

    private:
        enum class SortMode
        {
            RecentlyModified,
            All,
        };

        enum class Type
        {
            Owned,
            Shared,
        };

        void refreshView();
        void addSome();
        void addTracklist(const db::ObjectPtr<db::TrackList>& trackList);

        static constexpr SortMode _defaultSortMode{ SortMode::RecentlyModified };
        static constexpr Type _defaultType{ Type::Owned };
        static constexpr std::size_t _batchSize{ 30 };
        static constexpr std::size_t _maxCount{ 500 };

        SortMode _sortMode{ _defaultSortMode };
        Type _type{ _defaultType };
        std::string _searchText;
        Filters& _filters;
        InfiniteScrollingContainer* _container{};
        std::unordered_map<db::TrackListId, Wt::WWidget*> _trackListWidgets;
    };
} // namespace lms::ui
