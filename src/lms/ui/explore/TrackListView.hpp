
#pragma once

#include "database/objects/TrackListId.hpp"

#include "common/Template.hpp"

namespace lms::ui
{
    class Filters;
    class InfiniteScrollingContainer;
    class PlayQueueController;

    class TrackList : public Template
    {
    public:
        TrackList(Filters& filters, PlayQueueController& playQueueController);

        Wt::Signal<db::TrackListId> trackListDeleted;

    private:
        void refreshView();
        void addSome();

        static constexpr std::size_t _batchSize{ 6 };
        static constexpr std::size_t _maxCount{ 8000 };

        Filters& _filters;
        PlayQueueController& _playQueueController;
        db::TrackListId _trackListId;
        InfiniteScrollingContainer* _container{};
    };
} // namespace lms::ui
