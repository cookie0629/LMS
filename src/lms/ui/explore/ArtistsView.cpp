
#include "ArtistsView.hpp"

#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>

#include "database/Session.hpp"
#include "database/objects/Artist.hpp"
#include "database/objects/TrackArtistLink.hpp"

#include "ArtistListHelpers.hpp"
#include "Filters.hpp"
#include "LmsApplication.hpp"
#include "SortModeSelector.hpp"
#include "State.hpp"
#include "TrackArtistLinkTypeSelector.hpp"
#include "common/InfiniteScrollingContainer.hpp"

namespace lms::ui
{
    Artists::Artists(Filters& filters)
        : Template{ Wt::WString::tr("Lms.Explore.Artists.template") }
        , _artistCollector{ filters, _defaultSortMode, _maxCount }
    {
        addFunction("tr", &Wt::WTemplate::Functions::tr);
        addFunction("id", &Wt::WTemplate::Functions::id);

        Wt::WLineEdit* searchEdit{ bindNew<Wt::WLineEdit>("search") };
        searchEdit->setPlaceholderText(Wt::WString::tr("Lms.Explore.Search.search-placeholder"));
        searchEdit->textInput().connect([this, searchEdit] {
            refreshView(searchEdit->text());
        });

        {
            const ArtistCollector::Mode sortMode{ state::readValue<ArtistCollector::Mode>("artists_sort_mode").value_or(_defaultSortMode) };
            _artistCollector.setMode(sortMode);

            SortModeSelector* sortModeSelector{ bindNew<SortModeSelector>("sort-mode", sortMode) };
            sortModeSelector->itemSelected.connect([this](ArtistCollector::Mode newSortMode) {
                state::writeValue<ArtistCollector::Mode>("artists_sort_mode", newSortMode);
                refreshView(newSortMode);
            });
        }

        {
            const std::optional<db::TrackArtistLinkType> linkType{ state::readValue<db::TrackArtistLinkType>("artists_link_type") };
            _artistCollector.setArtistLinkType(linkType);

            TrackArtistLinkTypeSelector* linkTypeSelector{ bindNew<TrackArtistLinkTypeSelector>("link-type", linkType) };
            linkTypeSelector->itemSelected.connect([this](std::optional<db::TrackArtistLinkType> newLinkType) {
                state::writeValue<db::TrackArtistLinkType>("artists_link_type", newLinkType);
                refreshView(newLinkType);
            });
        }

        _container = bindNew<InfiniteScrollingContainer>("artists", Wt::WString::tr("Lms.Explore.Artists.template.container"));
        _container->onRequestElements.connect([this] {
            addSome();
        });

        filters.updated().connect([this] {
            refreshView();
        });

        refreshView(_artistCollector.getMode());
    }

    void Artists::refreshView()
    {
        _container->reset();
        _artistCollector.reset();
    }

    void Artists::refreshView(ArtistCollector::Mode mode)
    {
        _artistCollector.setMode(mode);
        refreshView();
    }

    void Artists::refreshView(std::optional<db::TrackArtistLinkType> linkType)
    {
        _artistCollector.setArtistLinkType(linkType);
        refreshView();
    }

    void Artists::refreshView(const Wt::WString& searchText)
    {
        _artistCollector.setSearch(searchText.toUTF8());
        refreshView();
    }

    void Artists::addSome()
    {
        const auto artistIds{ _artistCollector.get(db::Range{ static_cast<std::size_t>(_container->getCount()), _batchSize }) };

        {
            auto transaction{ LmsApp->getDbSession().createReadTransaction() };

            for (const db::ArtistId artistId : artistIds.results)
            {
                if (const auto artist{ db::Artist::find(LmsApp->getDbSession(), artistId) })
                    _container->add(ArtistListHelpers::createEntry(artist));
            }
        }

        _container->setHasMore(artistIds.moreResults);
    }

} // namespace lms::ui
