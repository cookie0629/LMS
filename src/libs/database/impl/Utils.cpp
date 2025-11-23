#include "database/Utils.hpp"

#include <Wt/Dbo/Query.h>
#include <Wt/Dbo/Session.h>

#include "database/Session.hpp"

namespace lms::db::utils
{
    void executeCommand(Session& session, const std::string& sql)
    {
        // 通过 Session 的 execute 方法执行SQL
        session.execute(sql);
    }

    // 模板函数在头文件中实现
} // namespace lms::db::utils

