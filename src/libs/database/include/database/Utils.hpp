#pragma once

#include <functional>
#include <optional>
#include <string>

#include <Wt/Dbo/Query.h>

namespace lms::db
{
    class Session;
} // namespace lms::db

namespace lms::db::utils
{
    /**
     * @brief 执行SQL命令
     */
    void executeCommand(Session& session, const std::string& sql);

    /**
     * @brief 获取查询的单个结果
     * @tparam T 结果类型
     * @param query 查询对象
     * @return 查询结果，如果没有结果则返回 std::nullopt
     */
    template<typename T>
    std::optional<T> fetchQuerySingleResult(Wt::Dbo::Query<T>& query)
    {
        auto results = query.resultList();
        if (results.empty())
            return std::nullopt;
        return results.front();
    }

    /**
     * @brief 遍历查询结果
     * @tparam T 结果类型
     * @param query 查询对象
     * @param visitor 访问器函数
     */
    template<typename T>
    void forEachQueryResult(Wt::Dbo::Query<T>& query, std::function<void(const T&)> visitor)
    {
        auto results = query.resultList();
        for (const auto& result : results)
        {
            visitor(result);
        }
    }
} // namespace lms::db::utils

