
#include "database/objects/MediaLibrary.hpp"

#include <Wt/Dbo/Impl.h>

#include "database/Session.hpp"
#include "database/objects/Track.hpp"

#include "Utils.hpp"
#include "traits/IdTypeTraits.hpp"
#include "traits/PathTraits.hpp"
#include "traits/StringViewTraits.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::MediaLibrary)

namespace lms::db
{
    MediaLibrary::MediaLibrary(std::string_view name, const std::filesystem::path& p)
        : _name{ std::string{ name, 0, maxNameLength } }
    {
        setPath(p);
    }

    MediaLibrary::pointer MediaLibrary::create(Session& session, std::string_view name, const std::filesystem::path& p)
    {
        return session.getDboSession()->add(std::unique_ptr<MediaLibrary>{ new MediaLibrary{ name, p } });
    }

    std::size_t MediaLibrary::getCount(Session& session)
    {
        session.checkReadTransaction();

        return utils::fetchQuerySingleResult(session.getDboSession()->query<int>("SELECT COUNT(*) FROM media_library"));
    }

    MediaLibrary::pointer MediaLibrary::find(Session& session, MediaLibraryId id)
    {
        session.checkReadTransaction();

        return utils::fetchQuerySingleResult(session.getDboSession()->find<MediaLibrary>().where("id = ?").bind(id));
    }

    MediaLibrary::pointer MediaLibrary::find(Session& session, std::string_view name)
    {
        session.checkReadTransaction();

        return utils::fetchQuerySingleResult(session.getDboSession()->find<MediaLibrary>().where("name = ?").bind(name));
    }

    MediaLibrary::pointer MediaLibrary::find(Session& session, const std::filesystem::path& p)
    {
        session.checkReadTransaction();

        return utils::fetchQuerySingleResult(session.getDboSession()->find<MediaLibrary>().where("path = ?").bind(p));
    }

    void MediaLibrary::find(Session& session, std::function<void(const MediaLibrary::pointer&)> func)
    {
        session.checkReadTransaction();

        utils::forEachQueryResult(session.getDboSession()->find<MediaLibrary>(), [&](const MediaLibrary::pointer& mediaLibrary) {
            func(mediaLibrary);
        });
    }

    bool MediaLibrary::isEmpty() const
    {
        assert(session());
        auto query{ session()->query<bool>("SELECT EXISTS (SELECT 1 FROM track WHERE media_library_id = ? LIMIT 1) AS is_media_library_empty").bind(getId()) };
        return !utils::fetchQuerySingleResult(query);
    }

    void MediaLibrary::setPath(const std::filesystem::path& p)
    {
        assert(p.is_absolute());
        if (!p.has_filename() && p.has_parent_path())
            _path = p.parent_path();
        else
            _path = p;
    }
} // namespace lms::db
