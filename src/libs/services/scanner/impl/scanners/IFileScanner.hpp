
#pragma once

#include <filesystem>
#include <span>

#include "core/LiteralString.hpp"

#include "FileToScan.hpp"

namespace lms::scanner
{
    class IFileScanOperation;

    class IFileScanner
    {
    public:
        virtual ~IFileScanner() = default;

        virtual core::LiteralString getName() const = 0;
        virtual std::span<const std::filesystem::path> getSupportedFiles() const = 0;
        virtual std::span<const std::filesystem::path> getSupportedExtensions() const = 0;
        virtual bool needsScan(const FileToScan& file) const = 0;
        virtual std::unique_ptr<IFileScanOperation> createScanOperation(FileToScan&& fileToScan) const = 0;
    };
} // namespace lms::scanner