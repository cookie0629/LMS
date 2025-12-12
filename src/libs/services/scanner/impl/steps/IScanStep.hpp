
#pragma once

#include "core/LiteralString.hpp"
#include "services/scanner/ScannerStats.hpp"

namespace lms::scanner
{
    struct ScanContext;

    // IScanStep: 扫描流程中的一个步骤（如扫描文件、检查缺失文件等）的抽象接口。
    // IScanStep: абстракция одного шага сканирования (скан файлов, проверка удалённых файлов и т.п.).
    class IScanStep
    {
    public:
        virtual ~IScanStep() = default;

        virtual ScanStep getStep() const = 0;
        virtual core::LiteralString getStepName() const = 0;
        virtual bool needProcess(const ScanContext& context) const = 0;
        virtual void process(ScanContext& context) = 0;
    };
} // namespace lms::scanner
