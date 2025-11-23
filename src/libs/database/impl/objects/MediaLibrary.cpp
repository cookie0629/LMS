#include "database/objects/MediaLibrary.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/Track.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::MediaLibrary)

namespace lms::db
{
    MediaLibrary::MediaLibrary(std::string_view name, const std::filesystem::path& p)
        : _name{ name.length() > maxNameLength ? std::string{ name.substr(0, maxNameLength) } : std::string{ name } }
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
        return session.getDboSession()->find<MediaLibrary>().resultList().size();
    }

    MediaLibrary::pointer MediaLibrary::find(Session& session, MediaLibraryId id)
    {
        session.checkReadTransaction();
        Wt::Dbo::ptr<MediaLibrary> ptr = session.getDboSession()->find<MediaLibrary>()
            .where("id = ?")
            .bind(id.getValue());
        return ptr ? MediaLibrary::pointer(ptr) : MediaLibrary::pointer();
    }

    MediaLibrary::pointer MediaLibrary::find(Session& session, std::string_view name)
    {
        session.checkReadTransaction();
        Wt::Dbo::ptr<MediaLibrary> ptr = session.getDboSession()->find<MediaLibrary>()
            .where("name = ?")
            .bind(std::string{ name });
        return ptr ? MediaLibrary::pointer(ptr) : MediaLibrary::pointer();
    }

    MediaLibrary::pointer MediaLibrary::find(Session& session, const std::filesystem::path& path)
    {
        session.checkReadTransaction();
        Wt::Dbo::ptr<MediaLibrary> ptr = session.getDboSession()->find<MediaLibrary>()
            .where("path = ?")
            .bind(path.string());
        return ptr ? MediaLibrary::pointer(ptr) : MediaLibrary::pointer();
    }

    void MediaLibrary::find(Session& session, std::function<void(const MediaLibrary::pointer&)> func)
    {
        session.checkReadTransaction();
        auto results = session.getDboSession()->find<MediaLibrary>().resultList();
        for (const auto& ptr : results)
        {
            func(MediaLibrary::pointer(ptr));
        }
    }

    std::vector<MediaLibrary::pointer> MediaLibrary::findAll(Session& session)
    {
        session.checkReadTransaction();
        std::vector<MediaLibrary::pointer> result;
        auto results = session.getDboSession()->find<MediaLibrary>().resultList();
        for (const auto& ptr : results)
        {
            result.push_back(MediaLibrary::pointer(ptr));
        }
        return result;
    }

    MediaLibrary::pointer MediaLibrary::getOrCreate(Session& session, std::string_view name, const std::filesystem::path& path)
    {
        session.checkWriteTransaction();
        
        auto library = find(session, path);
        if (!library)
        {
            library = create(session, name, path);
        }
        return library;
    }

    void MediaLibrary::setName(std::string_view name)
    {
        if (name.length() > maxNameLength)
        {
            _name = std::string{ name.substr(0, maxNameLength) };
        }
        else
        {
            _name = std::string{ name };
        }
    }

    void MediaLibrary::setPath(const std::filesystem::path& p)
    {
        _path = p.string();
    }

    bool MediaLibrary::isEmpty() const
    {
        // 简化实现：检查是否有任何 Track 关联到此 MediaLibrary
        // 实际实现需要查询数据库
        return false;
    }
} // namespace lms::db

