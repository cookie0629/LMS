
#include "database/IdType.hpp"

#include <cassert>
#include <type_traits>

#include <Wt/Dbo/ptr.h>

namespace lms::db
{
    static_assert(std::is_same_v<IdType::ValueType, Wt::Dbo::dbo_default_traits::IdType>);

    IdType::IdType()
        : _id{ Wt::Dbo::dbo_default_traits::invalidId() }
    {
    }

    IdType::IdType(ValueType id)
        : _id{ id }
    {
    }

    bool IdType::isValid() const
    {
        return _id != Wt::Dbo::dbo_default_traits::invalidId();
    }

    std::string IdType::toString() const
    {
        assert(isValid());
        return std::to_string(_id);
    }

} // namespace lms::db
