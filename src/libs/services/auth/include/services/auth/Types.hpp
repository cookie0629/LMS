#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

#include "database/Types.hpp"

namespace lms::auth
{
    /**
     * @brief Auth Service 异常基类
     */
    class Exception : public std::runtime_error
    {
    public:
        explicit Exception(const std::string& message)
            : std::runtime_error(message) {}
    };

    /**
     * @brief 用户未找到异常
     */
    class UserNotFoundException : public Exception
    {
    public:
        UserNotFoundException()
            : Exception("User not found") {}
    };

    /**
     * @brief 密码验证上下文
     */
    struct PasswordValidationContext
    {
        std::string loginName;
        db::UserType userType{ db::UserType::REGULAR };
    };
} // namespace lms::auth
