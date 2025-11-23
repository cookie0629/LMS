#pragma once

namespace lms::scanner
{
    /**
     * @brief 扫描选项
     */
    struct ScanOptions
    {
        bool fullScan{};      // 全量扫描：即使文件未更改也扫描
        bool forceOptimize{}; // 强制优化数据库
        bool compact{};       // 压缩数据库
    };
} // namespace lms::scanner

