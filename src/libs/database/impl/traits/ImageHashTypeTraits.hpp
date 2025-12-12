
#pragma once

#include <Wt/Dbo/StdSqlTraits.h>

#include "core/String.hpp"
#include "database/Types.hpp"

namespace Wt::Dbo
{
    template<>
    struct sql_value_traits<lms::db::ImageHashType>
    {
        static const bool specialized = true;
        // Uses an underlying string to encode this big value

        static std::string type(SqlConnection* conn, int size)
        {
            return sql_value_traits<std::string, void>::type(conn, size);
        }

        static void bind(const lms::db::ImageHashType& v, SqlStatement* statement, int column, int size)
        {
            std::string valueAsStr{ std::to_string(v.value()) };
            sql_value_traits<std::string>::bind(valueAsStr, statement, column, size);
        }

        static bool read(lms::db::ImageHashType& v, SqlStatement* statement, int column, int size)
        {
            std::string valueAsStr;
            if (sql_value_traits<std::string>::read(valueAsStr, statement, column, size))
            {
                if (const auto parsedValue{ lms::core::stringUtils::readAs<lms::db::ImageHashType::underlying_type>(valueAsStr) })
                {
                    v = lms::db::ImageHashType{ *parsedValue };
                    return true;
                }
            }

            v = lms::db::ImageHashType{};
            return false;
        }
    };
} // namespace Wt::Dbo
