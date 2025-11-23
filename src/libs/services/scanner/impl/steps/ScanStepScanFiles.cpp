#include "ScanStepScanFiles.hpp"

#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"

namespace lms::scanner
{
    ScanStepScanFiles::ScanStepScanFiles(db::IDb& db, const ScannerSettings& settings)
        : ScanStepBase{ db, settings }
    {
    }

    bool ScanStepScanFiles::execute(ScanStats& /* stats */)
    {
        LMS_LOG(SCANNER, INFO, "Starting file scan step");

        // 简化版：暂时只记录日志
        // 实际实现需要：
        // 1. 遍历媒体库目录
        // 2. 查找音频文件
        // 3. 创建 FileToScan 对象
        // 4. 使用 IFileScanner 扫描文件
        // 5. 更新数据库

        LMS_LOG(SCANNER, INFO, "File scan step completed");

        return true;
    }
} // namespace lms::scanner

