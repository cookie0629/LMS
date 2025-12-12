// 数据库工具函数实现

#include "Utils.hpp"

#include "core/String.hpp"

namespace lms::db::utils
{
    std::string escapeForLikeKeyword(std::string_view keyword)
    {
        return core::stringUtils::escapeString(keyword, "%_", escapeChar);
    }

    Wt::WDateTime normalizeDateTime(const Wt::WDateTime& dateTime)
    {
        // force second resolution
        return Wt::WDateTime::fromTime_t(dateTime.toTime_t());
    }
} // namespace lms::db::utils
