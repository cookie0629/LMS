#include "database/Utils.hpp"

#include "database/Session.hpp"

namespace lms::db::utils
{
    void executeCommand(Session& session, const std::string& sql)
    {
        // 通过 Session 的 execute 方法执行SQL
        session.execute(sql);
    }
} // namespace lms::db::utils

