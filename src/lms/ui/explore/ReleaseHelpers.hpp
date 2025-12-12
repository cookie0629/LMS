
#pragma once

#include <memory>
#include <optional>

#include <Wt/WDate.h>
#include <Wt/WString.h>
#include <Wt/WTemplate.h>

#include "database/Object.hpp"

#include "ReleaseTypes.hpp"

namespace lms::db
{
    class Artist;
    class Release;
} // namespace lms::db

namespace lms::ui::releaseListHelpers
{
    std::unique_ptr<Wt::WTemplate> createEntry(const db::ObjectPtr<db::Release>& release);
    std::unique_ptr<Wt::WTemplate> createEntryForArtist(const db::ObjectPtr<db::Release>& release, const db::ObjectPtr<db::Artist>& artist);
    std::unique_ptr<Wt::WTemplate> createEntryForOtherVersions(const db::ObjectPtr<db::Release>& release);
} // namespace lms::ui::releaseListHelpers

namespace lms::ui::releaseHelpers
{
    Wt::WString buildReleaseTypeString(const ReleaseType& releaseType);
    Wt::WString buildReleaseYearString(std::optional<int> year, std::optional<int> originalYear);
} // namespace lms::ui::releaseHelpers
