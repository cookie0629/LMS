
#pragma once

#include <optional>

#include "database/Types.hpp"

#include "DropDownMenuSelector.hpp"

namespace lms::ui
{
    class TrackArtistLinkTypeSelector : public DropDownMenuSelector<std::optional<db::TrackArtistLinkType>>
    {
    public:
        TrackArtistLinkTypeSelector(std::optional<db::TrackArtistLinkType> defaultLinkType)
            : DropDownMenuSelector{ Wt::WString::tr("Lms.Explore.Artists.template.track-artist-link-type-selector"), defaultLinkType }
        {
            bindItem("link-type-all", Wt::WString::tr("Lms.Explore.Artists.linktype-all"), std::nullopt);
            bindItem("link-type-artist", Wt::WString::trn("Lms.Explore.Artists.linktype-artist", 2), db::TrackArtistLinkType::Artist);
            bindItem("link-type-releaseartist", Wt::WString::trn("Lms.Explore.Artists.linktype-releaseartist", 2), db::TrackArtistLinkType::ReleaseArtist);
            bindItem("link-type-composer", Wt::WString::trn("Lms.Explore.Artists.linktype-composer", 2), db::TrackArtistLinkType::Composer);
            bindItem("link-type-conductor", Wt::WString::trn("Lms.Explore.Artists.linktype-conductor", 2), db::TrackArtistLinkType::Conductor);
            bindItem("link-type-lyricist", Wt::WString::trn("Lms.Explore.Artists.linktype-lyricist", 2), db::TrackArtistLinkType::Lyricist);
            bindItem("link-type-mixer", Wt::WString::trn("Lms.Explore.Artists.linktype-mixer", 2), db::TrackArtistLinkType::Mixer);
            bindItem("link-type-performer", Wt::WString::trn("Lms.Explore.Artists.linktype-performer", 2), db::TrackArtistLinkType::Performer);
            bindItem("link-type-producer", Wt::WString::trn("Lms.Explore.Artists.linktype-producer", 2), db::TrackArtistLinkType::Producer);
            bindItem("link-type-remixer", Wt::WString::trn("Lms.Explore.Artists.linktype-remixer", 2), db::TrackArtistLinkType::Remixer);
        }
    };
} // namespace lms::ui
