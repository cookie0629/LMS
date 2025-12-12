
#pragma once

#include "services/scanner/ScannerOptions.hpp"
#include "services/scanner/ScannerStats.hpp"

namespace lms::scanner
{
    // ScanContext: 一次扫描过程中在各个步骤之间传递的共享上下文（选项 + 全局统计 + 当前步骤统计）。
    // ScanContext: общий контекст одного запуска сканера (опции, глобальная статистика и статистика текущего шага).
    struct ScanContext
    {
        ScanOptions scanOptions;
        ScanStats stats;
        ScanStepStats currentStepStats;
    };
} // namespace lms::scanner