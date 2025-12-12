
#include "services/scanner/ScannerStats.hpp"

namespace lms::scanner
{
    std::size_t ScanStats::getTotalFileCount() const
    {
        return skips + additions + updates + failures;
    }

    std::size_t ScanStats::getChangesCount() const
    {
        return additions + deletions + updates;
    }

    unsigned ScanStepStats::progress() const
    {
        const unsigned res{ static_cast<unsigned>((processedElems / static_cast<float>(totalElems ? totalElems : 1)) * 100) };
        // 理论上可能略高于 100%，因为遍历文件系统时总数可能在增长。
        // Теоретически может немного превышать 100%, так как во время обхода ФС может добавляться больше файлов.
        return res;
    }
} // namespace lms::scanner
