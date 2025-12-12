// 子进程抽象接口
#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <vector>

#include "core/Exception.hpp"

namespace lms::core
{
    // ChildProcessException: ошибки, связанные с запуском或读取子进程。
    // ChildProcessException: исключения, связанные со spawning/чтением дочерних процессов.
    class ChildProcessException : public LmsException
    {
    public:
        using LmsException::LmsException;
    };

    // IChildProcess: абстракция над子进程，用于异步/同步读取其标准输出。
    // IChildProcess：对子进程的抽象，用于异步/同步读取 stdout。
    class IChildProcess
    {
    public:
        using Args = std::vector<std::string>;

        virtual ~IChildProcess() = default;

        enum class ReadResult
        {
            Success,
            Error,
            EndOfFile,
        };

        using ReadCallback = std::function<void(ReadResult, std::size_t)>;
        virtual void asyncRead(std::byte* data, std::size_t bufferSize, ReadCallback callback) = 0;

        virtual std::size_t readSome(std::byte* data, std::size_t bufferSize) = 0;
        virtual bool finished() const = 0;
    };
} // namespace lms::core