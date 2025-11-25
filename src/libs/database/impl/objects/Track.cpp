#include "database/objects/Track.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "core/Path.hpp"
#include "database/Object.hpp"
#include "database/objects/Directory.hpp"
#include "database/objects/Medium.hpp"
#include "database/objects/Release.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::Track)

namespace lms::db
{
    Track::Track(const std::filesystem::path& filePath)
        : _absoluteFilePath{ filePath.string() }
        , _name{ core::pathUtils::getFilename(filePath) }
    {
        if (_name.length() > maxNameLength)
        {
            _name.resize(maxNameLength);
        }
    }

    Track::pointer Track::create(Session& session, const std::filesystem::path& filePath)
    {
        return session.getDboSession()->add(std::unique_ptr<Track>{ new Track{ filePath } });
    }

    std::size_t Track::getCount(Session& session)
    {
        session.checkReadTransaction();
        return session.getDboSession()->find<Track>().resultList().size();
    }

    Track::pointer Track::find(Session& session, TrackId id)
    {
        session.checkReadTransaction();
        Wt::Dbo::ptr<Track> ptr = session.getDboSession()->find<Track>()
            .where("id = ?")
            .bind(id.getValue());
        return ptr ? Track::pointer(ptr) : Track::pointer();
    }

    Track::pointer Track::findByPath(Session& session, const std::filesystem::path& path)
    {
        session.checkReadTransaction();
        Wt::Dbo::ptr<Track> ptr = session.getDboSession()->find<Track>()
            .where("file_path = ?")
            .bind(path.string());
        return ptr ? Track::pointer(ptr) : Track::pointer();
    }

    void Track::find(Session& session, std::function<void(const Track::pointer&)> func)
    {
        session.checkReadTransaction();
        auto results = session.getDboSession()->find<Track>().resultList();
        for (const auto& ptr : results)
        {
            func(Track::pointer(ptr));
        }
    }

    Track::pointer Track::getOrCreate(Session& session, const std::filesystem::path& path)
    {
        session.checkWriteTransaction();
        
        auto track = findByPath(session, path);
        if (!track)
        {
            track = create(session, path);
        }
        return track;
    }

    void Track::setName(std::string_view name)
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

    void Track::setAbsoluteFilePath(const std::filesystem::path& filePath)
    {
        _absoluteFilePath = filePath.string();
    }

    ReleaseId Track::getReleaseId() const
    {
        if (!_release)
            return ReleaseId{};
        return ReleaseId{ static_cast<ReleaseId::ValueType>(_release.id()) };
    }

    void Track::setRelease(ObjectPtr<Release> release)
    {
        _release = Object<Track, TrackId>::getDboPtr(release);
    }

    MediumId Track::getMediumId() const
    {
        if (!_medium)
            return MediumId{};
        return MediumId{ static_cast<MediumId::ValueType>(_medium.id()) };
    }

    void Track::setMedium(ObjectPtr<Medium> medium)
    {
        _medium = Object<Track, TrackId>::getDboPtr(medium);
    }

    DirectoryId Track::getDirectoryId() const
    {
        if (!_directory)
            return DirectoryId{};
        return DirectoryId{ static_cast<DirectoryId::ValueType>(_directory.id()) };
    }

    void Track::setDirectory(ObjectPtr<Directory> directory)
    {
        _directory = Object<Track, TrackId>::getDboPtr(directory);
    }
} // namespace lms::db

