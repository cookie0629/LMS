#pragma once

namespace lms::scanner
{
    /**
     * @brief 扫描运行时设置
     */
    struct ScannerSettings
    {
        bool enableOptimize{ true }; // 是否执行优化步骤
        bool enableCompact{};        // 是否执行压缩步骤
    };
} // namespace lms::scanner


