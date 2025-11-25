#include "database/objects/Directory.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "core/Path.hpp"
#include "database/Object.hpp"
#include "database/objects/MediaLibrary.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::Directory)

namespace lms::db
{
    Directory::Directory(const std::filesystem::path& p)
        : _absolutePath{ p.string() }
        , _name{ core::pathUtils::getFilename(p) }
    {
    }

    Directory::pointer Directory::create(Session& session, const std::filesystem::path& p, ObjectPtr<MediaLibrary> mediaLibrary, ObjectPtr<Directory> parent)
    {
        auto dir = std::unique_ptr<Directory>{ new Directory{ p } };
        dir->_mediaLibrary = Object<Directory, DirectoryId>::getDboPtr(mediaLibrary);
        if (parent)
        {
            dir->_parent = Object<Directory, DirectoryId>::getDboPtr(parent);
        }
        return session.getDboSession()->add(std::move(dir));
    }

    std::size_t Directory::getCount(Session& session)
    {
        session.checkReadTransaction();
        return session.getDboSession()->find<Directory>().resultList().size();
    }

    Directory::pointer Directory::find(Session& session, DirectoryId id)
    {
        session.checkReadTransaction();
        Wt::Dbo::ptr<Directory> ptr = session.getDboSession()->find<Directory>()
            .where("id = ?")
            .bind(id.getValue());
        return ptr ? Directory::pointer(ptr) : Directory::pointer();
    }

    Directory::pointer Directory::find(Session& session, const std::filesystem::path& path)
    {
        session.checkReadTransaction();
        Wt::Dbo::ptr<Directory> ptr = session.getDboSession()->find<Directory>()
            .where("absolute_path = ?")
            .bind(path.string());
        return ptr ? Directory::pointer(ptr) : Directory::pointer();
    }

    void Directory::find(Session& session, std::function<void(const Directory::pointer&)> func)
    {
        session.checkReadTransaction();
        auto results = session.getDboSession()->find<Directory>().resultList();
        for (const auto& ptr : results)
        {
            func(Directory::pointer(ptr));
        }
    }

    Directory::pointer Directory::getOrCreate(Session& session, const std::filesystem::path& path, ObjectPtr<MediaLibrary> mediaLibrary, ObjectPtr<Directory> parent)
    {
        session.checkWriteTransaction();
        
        auto dir = find(session, path);
        if (!dir)
        {
            dir = create(session, path, mediaLibrary, parent);
        }
        return dir;
    }

    void Directory::setAbsolutePath(const std::filesystem::path& p)
    {
        _absolutePath = p.string();
        _name = core::pathUtils::getFilename(p);
    }

    DirectoryId Directory::getParentDirectoryId() const
    {
        if (!_parent)
            return DirectoryId{};
        return DirectoryId{ static_cast<DirectoryId::ValueType>(_parent.id()) };
    }

    void Directory::setParent(ObjectPtr<Directory> parent)
    {
        _parent = Object<Directory, DirectoryId>::getDboPtr(parent);
    }

    MediaLibraryId Directory::getMediaLibraryId() const
    {
        if (!_mediaLibrary)
            return MediaLibraryId{};
        return MediaLibraryId{ static_cast<MediaLibraryId::ValueType>(_mediaLibrary.id()) };
    }

    void Directory::setMediaLibrary(ObjectPtr<MediaLibrary> mediaLibrary)
    {
        _mediaLibrary = Object<Directory, DirectoryId>::getDboPtr(mediaLibrary);
    }
} // namespace lms::db

