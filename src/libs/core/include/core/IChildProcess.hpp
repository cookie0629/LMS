/*
 * Copyright (C) 2020 Emeric Poupon
 *
 * This file is part of LMS.
 *
 * LMS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LMS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LMS.  If not, see <http://www.gnu.org/licenses/>.
 */
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