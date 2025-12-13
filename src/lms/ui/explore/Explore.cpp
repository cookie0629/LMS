
#include "Explore.hpp"

#include <Wt/WApplication.h>
#include <Wt/WStackedWidget.h>

#include "ArtistView.hpp"
#include "ArtistsView.hpp"
#include "Filters.hpp"
#include "ReleaseView.hpp"
#include "ReleasesView.hpp"
#include "TracksView.hpp"

namespace lms::ui
{
    namespace
    {
        void handleContentsPathChange(Wt::WStackedWidget* stack)
        {
            enum Idx
            {
                IdxArtists = 0,
                IdxArtist,
                IdxReleases,
                IdxRelease,
                IdxTracks,
            };

            static const std::map<std::string, int> indexes = {
                { "/artists", IdxArtists },
                { "/artist", IdxArtist },
                { "/releases", IdxReleases },
                { "/release", IdxRelease },
                { "/tracks", IdxTracks },
            };

            for (const auto& index : indexes)
            {
                if (wApp->internalPathMatches(index.first))
                {
                    stack->setCurrentIndex(index.second);
                    return;
                }
            }
        }

    } // namespace

    Explore::Explore(Filters& filters, PlayQueue& playQueue)
        : Wt::WTemplate{ Wt::WString::tr("Lms.Explore.template") }
        , _playQueueController{ filters, playQueue }
    {
        addFunction("tr", &Functions::tr);

        // Contents
        Wt::WStackedWidget* contentsStack{ bindNew<Wt::WStackedWidget>("contents") };
        contentsStack->setOverflow(Wt::Overflow::Visible); // wt makes it hidden by default

        // same order as enum Idx
        auto artists = std::make_unique<Artists>(filters);
        contentsStack->addWidget(std::move(artists));

        auto artist = std::make_unique<Artist>(filters, _playQueueController);
        contentsStack->addWidget(std::move(artist));

        auto releases = std::make_unique<Releases>(filters, _playQueueController);
        contentsStack->addWidget(std::move(releases));

        auto release = std::make_unique<Release>(filters, _playQueueController);
        contentsStack->addWidget(std::move(release));

        auto tracks = std::make_unique<Tracks>(filters, _playQueueController);
        contentsStack->addWidget(std::move(tracks));

        wApp->internalPathChanged().connect(this, [contentsStack] {
            handleContentsPathChange(contentsStack);
        });

        handleContentsPathChange(contentsStack);
    }
} // namespace lms::ui
