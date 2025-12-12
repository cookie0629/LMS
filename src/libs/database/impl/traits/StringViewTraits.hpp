
#pragma once

#include <string_view>

#include <Wt/Dbo/SqlTraits.h>

namespace Wt::Dbo
{
    template<>
    struct sql_value_traits<std::string_view>
    {
        static void bind(std::string_view str, SqlStatement* statement, int column, int /* size */)
        {
            statement->bind(column, std::string{ str });
        }
    };
} // namespace Wt::Dbo
