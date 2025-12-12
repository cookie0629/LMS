
#pragma once

namespace lms::scanner
{
    // ScanOptions: 控制一次扫描行为的可选参数。
    // ScanOptions: опции, определяющие поведение одного прохода сканера.
    struct ScanOptions
    {
        bool fullScan{};      // 即使文件元数据未变也重新扫描 / пересканировать файлы даже без изменений
        bool forceOptimize{}; // 强制优化数据库 / принудительно оптимизировать БД
        bool compact{};       // 压缩数据库文件 / сжать файл базы данных
    };
} // namespace lms::scanner