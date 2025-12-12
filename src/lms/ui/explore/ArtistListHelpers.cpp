#include "ArtistListHelpers.hpp"

#include <Wt/WAnchor.h>

#include "database/objects/Artist.hpp"

#include "Utils.hpp"

namespace lms::ui::ArtistListHelpers
{
    std::unique_ptr<Wt::WTemplate> createEntry(const db::Artist::pointer& artist)
    {
        auto entry{ std::make_unique<Wt::WTemplate>(Wt::WString::tr("Lms.Explore.Artists.template.entry")) };
        entry->bindWidget("name", utils::createArtistAnchor(artist));

        Wt::WAnchor* anchor{ entry->bindWidget("image", utils::createArtistAnchor(artist, false)) };

        std::unique_ptr<Wt::WImage> image;
        if (artist->getPreferredArtworkId().isValid())
            image = utils::createArtworkImage(artist->getPreferredArtworkId(), ArtworkResource::DefaultArtworkType::Artist, ArtworkResource::Size::Large);
        else
            image = utils::createDefaultArtworkImage(ArtworkResource::DefaultArtworkType::Artist);

        image->addStyleClass("Lms-image-artist Lms-cover-anchor"); // hack
        anchor->setImage(std::move(image));

        return entry;
    }
} // namespace lms::ui::ArtistListHelpers