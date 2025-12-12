
#pragma once

#include <type_traits>

#include <Wt/Dbo/StdSqlTraits.h>

#include "database/Types.hpp"

namespace Wt::Dbo
{
    template<typename T>
    struct sql_value_traits<T, typename std::enable_if<std::is_base_of<lms::db::IdType, T>::value>::type>
    {
        static_assert(!std::is_same_v<lms::db::IdType, T>, "Cannot use IdType, use derived types");
        static const bool specialized = true;

        static std::string type(SqlConnection* conn, int size)
        {
            return sql_value_traits<typename T::ValueType, void>::type(conn, size);
        }

        static void bind(const T& v, SqlStatement* statement, int column, int size)
        {
            sql_value_traits<typename T::ValueType>::bind(v.getValue(), statement, column, size);
        }

        static bool read(T& v, SqlStatement* statement, int column, int size)
        {
            typename T::ValueType value;
            if (sql_value_traits<typename T::ValueType>::read(value, statement, column, size))
            {
                v = value;
                return true;
            }

            v = {};
            return false;
        }
    };
} // namespace Wt::Dbo
