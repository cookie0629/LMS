#pragma once

#include "core/ILogger.hpp"

#include <filesystem>
#include <fstream>
#include <iosfwd>
#include <list>
#include <mutex>
#include <unordered_map>

namespace lms::core::logging
{
    /**
     * @brief 日志器实现类
     */
    class Logger final : public ILogger
    {
    public:
        /**
         * @brief 构造函数
         * @param minSeverity 最小严重级别
         * @param logFilePath 日志文件路径（空表示输出到控制台）
         */
        Logger(Severity minSeverity, const std::filesystem::path& logFilePath);
        
        ~Logger() override;
        
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

    private:
        bool isSeverityActive(Severity severity) const override;
        void processLog(const Log& log) override;
        void processLog(Module module, Severity severity, std::string_view message) override;

        /**
         * @brief 添加输出流
         * @param os 输出流引用
         * @param severity 严重级别
         */
        void addOutputStream(std::ostream& os, Severity severity);

        /**
         * @brief 输出流包装器（包含互斥锁）
         */
        struct OutputStream
        {
            explicit OutputStream(std::ostream& os);

            std::mutex mutex;
            std::ostream& stream;
        };

        std::list<OutputStream> _outputStreams;
        std::unordered_map<Severity, OutputStream*> _severityToOutputStreamMap;
        std::unique_ptr<std::ofstream> _logFileStream;
    };
} // namespace lms::core::logging

