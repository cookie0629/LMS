#include "database/objects/Artist.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "core/String.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::Artist)

namespace lms::db
{
    Artist::Artist(std::string_view name)
        : _name{ name }
        , _sortName{ name }
    {
        if (_name.length() > maxNameLength)
        {
            _name.resize(maxNameLength);
        }
    }

    Artist::pointer Artist::create(Session& session, std::string_view name)
    {
        return session.getDboSession()->add(std::unique_ptr<Artist>{ new Artist{ name } });
    }

    std::size_t Artist::getCount(Session& session)
    {
        session.checkReadTransaction();
        return session.getDboSession()->find<Artist>().resultList().size();
    }

    Artist::pointer Artist::find(Session& session, ArtistId id)
    {
        session.checkReadTransaction();
        Wt::Dbo::ptr<Artist> ptr = session.getDboSession()->find<Artist>()
            .where("id = ?")
            .bind(id.getValue());
        return ptr ? Artist::pointer(ptr) : Artist::pointer();
    }

    Artist::pointer Artist::find(Session& session, std::string_view name)
    {
        session.checkReadTransaction();
        Wt::Dbo::ptr<Artist> ptr = session.getDboSession()->find<Artist>()
            .where("name = ?")
            .bind(std::string{ name });
        return ptr ? Artist::pointer(ptr) : Artist::pointer();
    }

    Artist::pointer Artist::getOrCreate(Session& session, std::string_view name)
    {
        session.checkWriteTransaction();
        
        auto artist = find(session, name);
        if (!artist)
        {
            artist = create(session, name);
        }
        return artist;
    }

    void Artist::setName(std::string_view name)
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

    void Artist::setSortName(std::string_view sortName)
    {
        if (sortName.length() > maxNameLength)
        {
            _sortName = std::string{ sortName.substr(0, maxNameLength) };
        }
        else
        {
            _sortName = std::string{ sortName };
        }
    }
} // namespace lms::db

