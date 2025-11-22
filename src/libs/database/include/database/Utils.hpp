#pragma once

#include <string>

namespace lms::db
{
    class Session;
}

namespace lms::db::utils
{
    /**
     * @brief 执行SQL命令
     */
    void executeCommand(Session& session, const std::string& sql);
} // namespace lms::db::utils

