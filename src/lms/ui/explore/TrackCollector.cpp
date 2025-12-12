
#include "TrackCollector.hpp"

#include "core/Service.hpp"
#include "database/Session.hpp"
#include "database/objects/Track.hpp"
#include "database/objects/TrackList.hpp"
#include "database/objects/User.hpp"
#include "services/feedback/IFeedbackService.hpp"
#include "services/scrobbling/IScrobblingService.hpp"

#include "Filters.hpp"
#include "LmsApplication.hpp"

namespace lms::ui
{
    db::RangeResults<db::TrackId> TrackCollector::get(std::optional<db::Range> requestedRange)
    {
        feedback::IFeedbackService& feedbackService{ *core::Service<feedback::IFeedbackService>::get() };
        scrobbling::IScrobblingService& scrobblingService{ *core::Service<scrobbling::IScrobblingService>::get() };

        const db::Range range{ getActualRange(requestedRange) };

        db::RangeResults<db::TrackId> tracks;

        switch (getMode())
        {
        case Mode::Random:
            tracks = getRandomTracks(range);
            break;

        case Mode::Starred:
            {
                feedback::IFeedbackService::FindParameters params;
                params.setFilters(getDbFilters());
                params.setKeywords(getSearchKeywords());
                params.setRange(range);
                params.setUser(LmsApp->getUserId());
                tracks = feedbackService.findStarredTracks(params);
                break;
            }

        case TrackCollector::Mode::RecentlyPlayed:
            {
                scrobbling::IScrobblingService::FindParameters params;
                params.setUser(LmsApp->getUserId());
                params.setFilters(getDbFilters());
                params.setKeywords(getSearchKeywords());
                params.setRange(range);

                tracks = scrobblingService.getRecentTracks(params);
                break;
            }

        case Mode::MostPlayed:
            {
                scrobbling::IScrobblingService::FindParameters params;
                params.setUser(LmsApp->getUserId());
                params.setFilters(getDbFilters());
                params.setKeywords(getSearchKeywords());
                params.setRange(range);

                tracks = scrobblingService.getTopTracks(params);
                break;
            }

        case Mode::RecentlyAdded:
            {
                db::Track::FindParameters params;
                params.setFilters(getDbFilters());
                params.setKeywords(getSearchKeywords());
                params.setSortMethod(db::TrackSortMethod::AddedDesc);
                params.setRange(range);

                {
                    auto transaction{ LmsApp->getDbSession().createReadTransaction() };
                    tracks = db::Track::findIds(LmsApp->getDbSession(), params);
                }
                break;
            }

        case Mode::RecentlyModified:
            {
                db::Track::FindParameters params;
                params.setFilters(getDbFilters());
                params.setKeywords(getSearchKeywords());
                params.setSortMethod(db::TrackSortMethod::LastWrittenDesc);
                params.setRange(range);

                {
                    auto transaction{ LmsApp->getDbSession().createReadTransaction() };
                    tracks = db::Track::findIds(LmsApp->getDbSession(), params);
                }
                break;
            }

        case Mode::All:
            {
                db::Track::FindParameters params;
                params.setFilters(getDbFilters());
                params.setKeywords(getSearchKeywords());
                params.setRange(range);

                {
                    auto transaction{ LmsApp->getDbSession().createReadTransaction() };
                    tracks = db::Track::findIds(LmsApp->getDbSession(), params);
                }
                break;
            }
        }

        if (range.offset + range.size == getMaxCount())
            tracks.moreResults = false;

        return tracks;
    }

    db::RangeResults<db::TrackId> TrackCollector::getRandomTracks(Range range)
    {
        assert(getMode() == Mode::Random);

        if (!_randomTracks)
        {
            db::Track::FindParameters params;
            params.setFilters(getDbFilters());
            params.setKeywords(getSearchKeywords());
            params.setSortMethod(db::TrackSortMethod::Random);
            params.setRange(db::Range{ 0, getMaxCount() });

            {
                auto transaction{ LmsApp->getDbSession().createReadTransaction() };
                _randomTracks = db::Track::findIds(LmsApp->getDbSession(), params);
            }
        }

        return _randomTracks->getSubRange(range);
    }

} // namespace lms::ui
