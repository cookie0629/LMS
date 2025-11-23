#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

namespace lms::audio
{
    /**
     * @brief 音频处理异常基类
     */
    class Exception : public std::runtime_error
    {
    public:
        explicit Exception(std::string_view message)
            : std::runtime_error{ std::string{ message } }
        {
        }
    };
} // namespace lms::audio

