#include "database/objects/Medium.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "database/Object.hpp"
#include "database/objects/Release.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::Medium)

namespace lms::db
{
    Medium::Medium(std::string_view name, ObjectPtr<Release> release, std::optional<int> position)
        : _name{ name }
        , _position{ position }
        , _release{ Object<Medium, MediumId>::getDboPtr(release) }
    {
        if (_name.length() > maxNameLength)
        {
            _name.resize(maxNameLength);
        }
    }

    Medium::pointer Medium::create(Session& session, std::string_view name, ObjectPtr<Release> release, std::optional<int> position)
    {
        return session.getDboSession()->add(std::unique_ptr<Medium>{ new Medium{ name, release, position } });
    }

    std::size_t Medium::getCount(Session& session)
    {
        session.checkReadTransaction();
        return session.getDboSession()->find<Medium>().resultList().size();
    }

    Medium::pointer Medium::find(Session& session, MediumId id)
    {
        session.checkReadTransaction();
        Wt::Dbo::ptr<Medium> ptr = session.getDboSession()->find<Medium>()
            .where("id = ?")
            .bind(id.getValue());
        return ptr ? Medium::pointer(ptr) : Medium::pointer();
    }

    Medium::pointer Medium::find(Session& session, ReleaseId releaseId, std::optional<int> position)
    {
        session.checkReadTransaction();
        auto query = session.getDboSession()->find<Medium>()
            .where("release_id = ?")
            .bind(releaseId.getValue());
        
        if (position.has_value())
        {
            query.where("position = ?").bind(*position);
        }
        
        Wt::Dbo::ptr<Medium> ptr = query;
        return ptr ? Medium::pointer(ptr) : Medium::pointer();
    }

    Medium::pointer Medium::getOrCreate(Session& session, ReleaseId releaseId, std::optional<int> position)
    {
        session.checkWriteTransaction();
        
        auto medium = find(session, releaseId, position);
        if (!medium)
        {
            auto release = Release::find(session, releaseId);
            if (!release)
            {
                return Medium::pointer();
            }
            
            std::string name = position.has_value() 
                ? ("Disc " + std::to_string(*position))
                : "Disc 1";
            
            medium = create(session, name, release, position);
        }
        return medium;
    }

    void Medium::setName(std::string_view name)
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

    ReleaseId Medium::getReleaseId() const
    {
        if (!_release)
            return ReleaseId{};
        return ReleaseId{ static_cast<ReleaseId::ValueType>(_release.id()) };
    }

    void Medium::setRelease(ObjectPtr<Release> release)
    {
        _release = Object<Medium, MediumId>::getDboPtr(release);
    }
} // namespace lms::db

