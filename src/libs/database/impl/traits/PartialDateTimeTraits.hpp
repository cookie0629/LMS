// 部分日期时间类型的序列化适配

#pragma once

#include "core/PartialDateTime.hpp"

#include <Wt/Dbo/SqlTraits.h>

namespace Wt::Dbo
{
    template<>
    struct sql_value_traits<lms::core::PartialDateTime>
    {
        static std::string type(SqlConnection* conn, int /*size*/)
        {
            return conn->dateTimeType(SqlDateTimeType::DateTime);
        }

        static void bind(const lms::core::PartialDateTime& dateTime, SqlStatement* statement, int column, int /* size */)
        {
            if (!dateTime.isValid())
                statement->bindNull(column);
            else
                statement->bind(column, dateTime.toString());
        }

        static bool read(lms::core::PartialDateTime& dateTime, SqlStatement* statement, int column, int size)
        {
            std::string str;
            if (!statement->getResult(column, &str, size))
                return false;

            dateTime = lms::core::PartialDateTime::fromString(str);
            return true;
        }
    };
} // namespace Wt::Dbo
