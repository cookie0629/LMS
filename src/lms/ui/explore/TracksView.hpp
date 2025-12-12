
#pragma once

#include "TrackCollector.hpp"
#include "common/Template.hpp"

namespace lms::ui
{
    class Filters;
    class InfiniteScrollingContainer;
    class PlayQueueController;

    class Tracks : public Template
    {
    public:
        Tracks(Filters& filters, PlayQueueController& playQueueController);

    private:
        void refreshView();
        void refreshView(TrackCollector::Mode mode);
        void refreshView(const Wt::WString& searchText);
        void addSome();

        std::vector<db::TrackId> getAllTracks();

        static constexpr TrackCollector::Mode _defaultMode{ TrackCollector::Mode::Random };
        static constexpr std::size_t _batchSize{ 6 };
        static constexpr std::size_t _maxCount{ 8000 };

        Filters& _filters;
        PlayQueueController& _playQueueController;
        InfiniteScrollingContainer* _container{};
        TrackCollector _trackCollector;
    };
} // namespace lms::ui
