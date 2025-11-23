#include "database/objects/Release.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "database/Session.hpp"
#include "database/Transaction.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::Release)

namespace lms::db
{
    Release::Release(std::string_view name)
        : _name{ name }
        , _sortName{ name }
    {
        if (_name.length() > maxNameLength)
        {
            _name.resize(maxNameLength);
        }
    }

    Release::pointer Release::create(Session& session, std::string_view name)
    {
        return session.getDboSession()->add(std::unique_ptr<Release>{ new Release{ name } });
    }

    std::size_t Release::getCount(Session& session)
    {
        session.checkReadTransaction();
        return session.getDboSession()->find<Release>().resultList().size();
    }

    Release::pointer Release::find(Session& session, ReleaseId id)
    {
        session.checkReadTransaction();
        Wt::Dbo::ptr<Release> ptr = session.getDboSession()->find<Release>()
            .where("id = ?")
            .bind(id.getValue());
        return ptr ? Release::pointer(ptr) : Release::pointer();
    }

    Release::pointer Release::find(Session& session, std::string_view name)
    {
        session.checkReadTransaction();
        Wt::Dbo::ptr<Release> ptr = session.getDboSession()->find<Release>()
            .where("name = ?")
            .bind(std::string{ name });
        return ptr ? Release::pointer(ptr) : Release::pointer();
    }

    Release::pointer Release::getOrCreate(Session& session, std::string_view name)
    {
        session.checkWriteTransaction();
        
        auto release = find(session, name);
        if (!release)
        {
            release = create(session, name);
        }
        return release;
    }

    void Release::setName(std::string_view name)
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

    void Release::setSortName(std::string_view sortName)
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

