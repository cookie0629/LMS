
#include "Filters.hpp"

#include <algorithm>
#include <optional>

#include "database/Session.hpp"
#include "database/objects/MediaLibrary.hpp"
#include "database/objects/Release.hpp"

#include "LmsApplication.hpp"
#include "State.hpp"
#include "Utils.hpp"

namespace lms::ui
{
    Filters::Filters()
        : Wt::WTemplate{ Wt::WString::tr("Lms.Explore.template.filters") }
    {
        addFunction("tr", &Wt::WTemplate::Functions::tr);

        // Filters
        _filters = bindNew<Wt::WContainerWidget>("clusters");

        if (const std::optional<db::MediaLibraryId::ValueType> mediaLibraryId{ state::readValue<db::MediaLibraryId::ValueType>("filters_media_library_id") })
            set(db::MediaLibraryId{ *mediaLibraryId });
        if (const std::optional<db::LabelId::ValueType> labelId{ state::readValue<db::LabelId::ValueType>("filters_label_id") })
            set(db::LabelId{ *labelId });
        if (const std::optional<db::ReleaseTypeId::ValueType> releaseTypeId{ state::readValue<db::ReleaseTypeId::ValueType>("filters_release_type_id") })
            set(db::ReleaseTypeId{ *releaseTypeId });
    }

    void Filters::add(db::ClusterId clusterId)
    {
        if (std::find(std::cbegin(_dbFilters.clusters), std::cend(_dbFilters.clusters), clusterId) != std::cend(_dbFilters.clusters))
            return;

        Wt::WInteractWidget* filter{};

        {
            auto cluster{ utils::createFilterCluster(clusterId, true) };
            if (!cluster)
                return;

            filter = _filters->addWidget(std::move(cluster));
        }

        _dbFilters.clusters.push_back(clusterId);

        filter->clicked().connect([this, filter, clusterId] {
            _filters->removeWidget(filter);
            _dbFilters.clusters.erase(std::remove_if(std::begin(_dbFilters.clusters), std::end(_dbFilters.clusters), [clusterId](db::ClusterId id) { return id == clusterId; }), std::end(_dbFilters.clusters));
            _sigUpdated.emit();
        });

        emitFilterAddedNotification();
    }

    void Filters::set(db::MediaLibraryId mediaLibraryId)
    {
        if (_mediaLibraryFilter)
        {
            _filters->removeWidget(_mediaLibraryFilter);
            _mediaLibraryFilter = nullptr;
            _dbFilters.mediaLibrary = db::MediaLibraryId{};
        }

        std::string libraryName;
        {
            auto transaction{ LmsApp->getDbSession().createReadTransaction() };

            const auto library{ db::MediaLibrary::find(LmsApp->getDbSession(), mediaLibraryId) };
            if (!library)
                return;

            libraryName = library->getName();
        }

        _dbFilters.mediaLibrary = mediaLibraryId;
        _mediaLibraryFilter = _filters->addWidget(utils::createFilter(Wt::WString::fromUTF8(libraryName), Wt::WString::tr("Lms.Explore.media-library"), "bg-primary", true));
        _mediaLibraryFilter->clicked().connect(_mediaLibraryFilter, [this] {
            _filters->removeWidget(_mediaLibraryFilter);
            _dbFilters.mediaLibrary = db::MediaLibraryId{};
            _mediaLibraryFilter = nullptr;
            _sigUpdated.emit();
            state::writeValue<db::MediaLibraryId::ValueType>("filters_media_library_id", std::nullopt);
        });

        emitFilterAddedNotification();
    }

    void Filters::set(db::LabelId labelId)
    {
        if (_labelFilter)
        {
            _filters->removeWidget(_labelFilter);
            _labelFilter = nullptr;
            _dbFilters.label = db::LabelId{};
        }

        std::string name;
        {
            auto transaction{ LmsApp->getDbSession().createReadTransaction() };

            const auto label{ db::Label::find(LmsApp->getDbSession(), labelId) };
            if (!label)
                return;

            name = label->getName();
        }

        _dbFilters.label = labelId;
        _labelFilter = _filters->addWidget(utils::createFilter(Wt::WString::fromUTF8(name), Wt::WString::tr("Lms.Explore.label"), "bg-secondary", true));
        _labelFilter->clicked().connect(_labelFilter, [this] {
            _filters->removeWidget(_labelFilter);
            _dbFilters.label = db::LabelId{};
            _labelFilter = nullptr;
            _sigUpdated.emit();
            state::writeValue<db::LabelId::ValueType>("filters_label_id", std::nullopt);
        });

        emitFilterAddedNotification();
    }

    void Filters::set(db::ReleaseTypeId releaseTypeId)
    {
        if (_releaseTypeFilter)
        {
            _filters->removeWidget(_releaseTypeFilter);
            _releaseTypeFilter = nullptr;
            _dbFilters.releaseType = db::ReleaseTypeId{};
        }

        std::string name;
        {
            auto transaction{ LmsApp->getDbSession().createReadTransaction() };

            const auto releaseType{ db::ReleaseType::find(LmsApp->getDbSession(), releaseTypeId) };
            if (!releaseType)
                return;

            name = releaseType->getName();
        }

        _dbFilters.releaseType = releaseTypeId;
        _releaseTypeFilter = _filters->addWidget(utils::createFilter(Wt::WString::fromUTF8(name), Wt::WString::tr("Lms.Explore.release-type"), "bg-dark", true));
        _releaseTypeFilter->clicked().connect(_releaseTypeFilter, [this] {
            _filters->removeWidget(_releaseTypeFilter);
            _dbFilters.releaseType = db::ReleaseTypeId{};
            _releaseTypeFilter = nullptr;
            _sigUpdated.emit();
            state::writeValue<db::ReleaseTypeId::ValueType>("filters_release_type_id", std::nullopt);
        });

        emitFilterAddedNotification();
    }

    void Filters::emitFilterAddedNotification()
    {
        _sigUpdated.emit();
    }
} // namespace lms::ui
