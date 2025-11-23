#pragma once

#include <filesystem>
#include <memory>
#include <string>

namespace lms::scanner
{
    /**
     * @brief 扫描错误基类
     */
    class ScanError
    {
    public:
        virtual ~ScanError() = default;

        virtual std::string getMessage() const = 0;
        virtual std::filesystem::path getPath() const = 0;
    };

    /**
     * @brief 文件扫描错误
     */
    class FileScanError : public ScanError
    {
    public:
        FileScanError(std::filesystem::path path, std::string message)
            : _path{ std::move(path) }
            , _message{ std::move(message) }
        {
        }

        std::string getMessage() const override { return _message; }
        std::filesystem::path getPath() const override { return _path; }

    private:
        std::filesystem::path _path;
        std::string _message;
    };

    /**
     * @brief 音频文件扫描错误
     */
    class AudioFileScanError : public FileScanError
    {
    public:
        AudioFileScanError(std::filesystem::path path, std::string message)
            : FileScanError{ std::move(path), std::move(message) }
        {
        }
    };
} // namespace lms::scanner

