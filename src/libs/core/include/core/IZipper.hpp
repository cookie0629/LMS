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

#include <filesystem>
#include <memory>
#include <vector>

#include "Exception.hpp"

namespace lms::zip
{
    // Entry: 描述要打包进 ZIP 的单个文件（逻辑名 + 物理路径）。
    // Entry: описывает один файл в ZIP‑архиве (логическое имя + путь на диске).
    struct Entry
    {
        std::string fileName;
        std::filesystem::path filePath;
    };
    using EntryContainer = std::vector<Entry>;

    // Exception: ZIP 打包相关错误（基于 LmsException）。
    // Exception: ошибки, связанные с работой ZIP‑архива, наследует LmsException.
    class Exception : public core::LmsException
    {
        using core::LmsException::LmsException;
    };

    // IZipper: 流式写入压缩档案的接口，用于按块写 ZIP 内容到输出流。
    // IZipper: интерфейс для потоковой записи ZIP‑архива в выходной поток.
    class IZipper
    {
    public:
        virtual ~IZipper() = default;

        virtual std::uint64_t writeSome(std::ostream& output) = 0;
        virtual bool isComplete() const = 0;
        virtual void abort() = 0;
    };

    // createArchiveZipper: 创建基于 libarchive 的 ZIP 实现。
    // createArchiveZipper: создаёт реализацию ZIP‑архива на базе libarchive.
    std::unique_ptr<IZipper> createArchiveZipper(const EntryContainer& entries);
} // namespace lms::zip
