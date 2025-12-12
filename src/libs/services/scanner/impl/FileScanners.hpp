
#pragma once

#include <filesystem>
#include <functional>
#include <unordered_map>
#include <vector>

namespace lms::scanner
{
    class IFileScanner;

    // FileScanners: 管理所有 IFileScanner，实现按文件名/扩展名快速选择合适的扫描器。
    // FileScanners: реестр IFileScanner, позволяет быстро подобрать сканер по имени файла или расширению.
    class FileScanners
    {
    public:
        void add(std::unique_ptr<IFileScanner> scanner);
        void clear();

        IFileScanner* select(const std::filesystem::path& filePath) const;
        void visit(const std::function<void(const IFileScanner&)>& visitor) const;

    private:
        std::unordered_map<std::filesystem::path, IFileScanner*> _scannerByFile;
        std::unordered_map<std::filesystem::path, IFileScanner*> _scannerByExtension;
        std::vector<std::unique_ptr<IFileScanner>> _fileScanners;
    };
} // namespace lms::scanner
