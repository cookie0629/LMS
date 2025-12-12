
#pragma once

#include "DropDownMenuSelector.hpp"

#include "DatabaseCollectorBase.hpp"

namespace lms::ui
{
    class SortModeSelector : public DropDownMenuSelector<DatabaseCollectorBase::Mode>
    {
    public:
        SortModeSelector(DatabaseCollectorBase::Mode defaultMode)
            : DropDownMenuSelector<DatabaseCollectorBase::Mode>{ Wt::WString::tr("Lms.Explore.template.sort-mode-selector"), defaultMode }
        {
            bindItem("random", Wt::WString::tr("Lms.Explore.random"), DatabaseCollectorBase::Mode::Random);
            bindItem("starred", Wt::WString::tr("Lms.Explore.starred"), DatabaseCollectorBase::Mode::Starred);
            bindItem("recently-played", Wt::WString::tr("Lms.Explore.recently-played"), DatabaseCollectorBase::Mode::RecentlyPlayed);
            bindItem("most-played", Wt::WString::tr("Lms.Explore.most-played"), DatabaseCollectorBase::Mode::MostPlayed);
            bindItem("recently-added", Wt::WString::tr("Lms.Explore.recently-added"), DatabaseCollectorBase::Mode::RecentlyAdded);
            bindItem("recently-modified", Wt::WString::tr("Lms.Explore.recently-modified"), DatabaseCollectorBase::Mode::RecentlyModified);
            bindItem("all", Wt::WString::tr("Lms.Explore.all"), DatabaseCollectorBase::Mode::All);
        }
    };
} // namespace lms::ui
