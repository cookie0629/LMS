
#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include "core/LiteralString.hpp"

namespace lms::scanner
{
    struct ScanError;

    class IFileScanOperation
    {
    public:
        virtual ~IFileScanOperation() = default;

        virtual core::LiteralString getName() const = 0;

        virtual const std::filesystem::path& getFilePath() const = 0;

        // scan() is called asynchronously by a pool of threads
        // processResult() is called sequentially by a single thread
        virtual void scan() = 0;

        enum class OperationResult
        {
            Added,
            Removed,
            Updated,
            Skipped,
        };
        virtual OperationResult processResult() = 0;

        using ScanErrorVector = std::vector<std::shared_ptr<ScanError>>;
        // list of errors collected during scan/result processing (there might be errors without skipping the file)
        virtual const ScanErrorVector& getErrors() = 0;
    };
} // namespace lms::scanner