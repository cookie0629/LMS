
#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

namespace lms::core
{
    // LmsException: базовый тип исключений внутри LMS（继承自 std::runtime_error）。
    // LmsException：LMS 内部使用的基础异常类型（继承自 std::runtime_error）。
    class LmsException : public std::runtime_error
    {
    public:
        LmsException(std::string_view error = "")
            : std::runtime_error{ std::string{ error } } {}
    };
} // namespace lms::core