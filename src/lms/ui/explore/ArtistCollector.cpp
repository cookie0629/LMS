
#include "ArtistCollector.hpp"

#include "core/Service.hpp"
#include "database/Session.hpp"
#include "database/objects/Artist.hpp"
#include "database/objects/TrackList.hpp"
#include "database/objects/User.hpp"
#include "services/feedback/IFeedbackService.hpp"
#include "services/scrobbling/IScrobblingService.hpp"

#include "Filters.hpp"
#include "LmsApplication.hpp"

namespace lms::ui
{
    db::RangeResults<db::ArtistId> ArtistCollector::get(std::optional<db::Range> requestedRange)
    {
        feedback::IFeedbackService& feedbackService{ *core::Service<feedback::IFeedbackService>::get() };
        scrobbling::IScrobblingService& scrobblingService{ *core::Service<scrobbling::IScrobblingService>::get() };

        const Range range{ getActualRange(requestedRange) };

        db::RangeResults<db::ArtistId> artists;

        switch (getMode())
        {
        case Mode::Random:
            artists = getRandomArtists(range);
            break;

        case Mode::Starred:
            {
                feedback::IFeedbackService::ArtistFindParameters params;
                params.setFilters(getDbFilters());
                params.setUser(LmsApp->getUserId());
                params.setKeywords(getSearchKeywords());
                params.setLinkType(_linkType);
                params.setSortMethod(db::ArtistSortMethod::StarredDateDesc);
                params.setRange(range);
                artists = feedbackService.findStarredArtists(params);
                break;
            }

        case Mode::RecentlyPlayed:
            {
                scrobbling::IScrobblingService::ArtistFindParameters params;
                params.setUser(LmsApp->getUserId());
                params.setFilters(getDbFilters());
                params.setKeywords(getSearchKeywords());
                params.setLinkType(_linkType);
                params.setRange(range);

                artists = scrobblingService.getRecentArtists(params);
                break;
            }

        case Mode::MostPlayed:
            {
                scrobbling::IScrobblingService::ArtistFindParameters params;
                params.setUser(LmsApp->getUserId());
                params.setFilters(getDbFilters());
                params.setKeywords(getSearchKeywords());
                params.setLinkType(_linkType);
                params.setRange(range);

                artists = scrobblingService.getTopArtists(params);
                break;
            }

        case Mode::RecentlyAdded:
            {
                db::Artist::FindParameters params;
                params.setFilters(getDbFilters());
                params.setKeywords(getSearchKeywords());
                params.setLinkType(_linkType);
                params.setSortMethod(db::ArtistSortMethod::AddedDesc);
                params.setRange(range);

                {
                    auto transaction{ LmsApp->getDbSession().createReadTransaction() };
                    artists = db::Artist::findIds(LmsApp->getDbSession(), params);
                }
                break;
            }

        case Mode::RecentlyModified:
            {
                db::Artist::FindParameters params;
                params.setFilters(getDbFilters());
                params.setKeywords(getSearchKeywords());
                params.setLinkType(_linkType);
                params.setSortMethod(db::ArtistSortMethod::LastWrittenDesc);
                params.setRange(range);

                {
                    auto transaction{ LmsApp->getDbSession().createReadTransaction() };
                    artists = db::Artist::findIds(LmsApp->getDbSession(), params);
                }
                break;
            }

        case Mode::All:
            {
                db::Artist::FindParameters params;
                params.setFilters(getDbFilters());
                params.setKeywords(getSearchKeywords());
                params.setLinkType(_linkType);
                params.setSortMethod(db::ArtistSortMethod::SortName);
                params.setRange(range);

                {
                    auto transaction{ LmsApp->getDbSession().createReadTransaction() };
                    artists = db::Artist::findIds(LmsApp->getDbSession(), params);
                }
                break;
            }
        }

        if (range.offset + range.size == getMaxCount())
            artists.moreResults = false;

        return artists;
    }

    db::RangeResults<db::ArtistId> ArtistCollector::getRandomArtists(Range range)
    {
        assert(getMode() == Mode::Random);

        if (!_randomArtists)
        {
            db::Artist::FindParameters params;
            params.setFilters(getDbFilters());
            params.setKeywords(getSearchKeywords());
            params.setLinkType(_linkType);
            params.setSortMethod(db::ArtistSortMethod::Random);
            params.setRange(db::Range{ 0, getMaxCount() });

            {
                auto transaction{ LmsApp->getDbSession().createReadTransaction() };
                _randomArtists = db::Artist::findIds(LmsApp->getDbSession(), params);
            }
        }

        return _randomArtists->getSubRange(range);
    }
} // namespace lms::ui
