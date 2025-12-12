
#pragma once

#include <memory>

#include <Wt/WTemplate.h>

#include "common/ValueStringModel.hpp"
#include "database/Object.hpp"
#include "database/Types.hpp"

namespace lms::db
{
    class Artist;
}

namespace lms::ui
{
    using ArtistLinkTypesModel = ValueStringModel<std::optional<db::TrackArtistLinkType>>;

    namespace ArtistListHelpers
    {
        std::unique_ptr<Wt::WTemplate> createEntry(const db::ObjectPtr<db::Artist>& artist);
    }
} // namespace lms::ui
