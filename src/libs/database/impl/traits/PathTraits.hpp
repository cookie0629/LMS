#pragma once

#include <filesystem>
#include <string>

#include <Wt/Dbo/SqlTraits.h>

namespace Wt::Dbo
{
    /**
     * @brief Wt::Dbo 对 std::filesystem::path 的支持
     */
    template<>
    struct sql_value_traits<std::filesystem::path>
    {
        static std::string type(SqlConnection* conn, int size)
        {
            return sql_value_traits<std::string>::type(conn, size);
        }

        static void bind(const std::filesystem::path& path, SqlStatement* statement, int column, int /* size */)
        {
            statement->bind(column, path.string());
        }

        static bool read(std::filesystem::path& p, SqlStatement* statement, int column, int size)
        {
            std::string s;
            bool result = statement->getResult(column, &s, size);
            if (!result)
                return false;

            p = std::filesystem::path{ s };

            return true;
        }
    };
} // namespace Wt::Dbo

