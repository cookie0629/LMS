#include "Logger.hpp"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

#include "core/String.hpp"

namespace lms::core::logging
{
    const char* getModuleName(Module mod)
    {
        switch (mod)
        {
        case Module::API_SUBSONIC:
            return "API_SUBSONIC";
        case Module::AUDIO:
            return "AUDIO";
        case Module::AUTH:
            return "AUTH";
        case Module::CHILDPROCESS:
            return "CHILDPROC";
        case Module::COVER:
            return "COVER";
        case Module::DB:
            return "DB";
        case Module::DBUPDATER:
            return "DB UPDATER";
        case Module::FEATURE:
            return "FEATURE";
        case Module::FEEDBACK:
            return "FEEDBACK";
        case Module::HTTP:
            return "HTTP";
        case Module::MAIN:
            return "MAIN";
        case Module::METADATA:
            return "METADATA";
        case Module::PODCAST:
            return "PODCAST";
        case Module::REMOTE:
            return "REMOTE";
        case Module::SCANNER:
            return "SCANNER";
        case Module::SCROBBLING:
            return "SCROBBLING";
        case Module::SERVICE:
            return "SERVICE";
        case Module::RECOMMENDATION:
            return "RECOMMENDATION";
        case Module::TRANSCODING:
            return "TRANSCODING";
        case Module::UI:
            return "UI";
        case Module::UTILS:
            return "UTILS";
        case Module::WT:
            return "WT";
        }
        return "";
    }

    const char* getSeverityName(Severity sev)
    {
        switch (sev)
        {
        case Severity::FATAL:
            return "fatal";
        case Severity::ERROR:
            return "error";
        case Severity::WARNING:
            return "warning";
        case Severity::INFO:
            return "info";
        case Severity::DEBUG_LEVEL:
            return "debug";
        }
        return "";
    }

    Log::Log(ILogger& logger, Module module, Severity severity)
        : _logger{ logger }
        , _module{ module }
        , _severity{ severity }
    {
    }

    Log::~Log()
    {
        assert(_logger.isSeverityActive(_severity));
        _logger.processLog(*this);
    }

    std::string Log::getMessage() const
    {
        return _oss.str();
    }

    std::unique_ptr<ILogger> createLogger(Severity minSeverity, const std::filesystem::path& logFilePath)
    {
        return std::make_unique<Logger>(minSeverity, logFilePath);
    }

    Logger::OutputStream::OutputStream(std::ostream& os)
        : stream{ os }
    {
    }

    Logger::Logger(Severity minSeverity, const std::filesystem::path& logFilePath)
    {
        // 如果指定了日志文件，打开文件流
        if (!logFilePath.empty())
        {
            _logFileStream = std::make_unique<std::ofstream>(logFilePath, std::ios::out | std::ios::app);
            if (!_logFileStream->is_open())
            {
                throw std::runtime_error("无法打开日志文件: " + logFilePath.string());
            }
        }

        // 根据最小严重级别设置输出流
        // 使用 fallthrough 特性，从最小级别开始，所有更严重的级别都会输出
        switch (minSeverity)
        {
        case Severity::DEBUG_LEVEL:
            if (_logFileStream)
                addOutputStream(*_logFileStream, Severity::DEBUG_LEVEL);
            else
                addOutputStream(std::cout, Severity::DEBUG_LEVEL);
            [[fallthrough]];
        case Severity::INFO:
            if (_logFileStream)
                addOutputStream(*_logFileStream, Severity::INFO);
            else
                addOutputStream(std::cout, Severity::INFO);
            [[fallthrough]];
        case Severity::WARNING:
            if (_logFileStream)
                addOutputStream(*_logFileStream, Severity::WARNING);
            else
                addOutputStream(std::cerr, Severity::WARNING);
            [[fallthrough]];
        case Severity::ERROR:
            if (_logFileStream)
                addOutputStream(*_logFileStream, Severity::ERROR);
            else
                addOutputStream(std::cerr, Severity::ERROR);
            [[fallthrough]];
        case Severity::FATAL:
            if (_logFileStream)
                addOutputStream(*_logFileStream, Severity::FATAL);
            else
                addOutputStream(std::cerr, Severity::FATAL);
            break;
        }
    }

    Logger::~Logger() = default;

    void Logger::addOutputStream(std::ostream& os, Severity severity)
    {
        // 查找是否已存在该输出流
        auto it = std::find_if(_outputStreams.begin(), _outputStreams.end(),
            [&os](const OutputStream& outputStream) {
                return &outputStream.stream == &os;
            });

        // 如果不存在，创建新的输出流
        if (it == _outputStreams.end())
        {
            it = _outputStreams.emplace(_outputStreams.end(), os);
        }

        // 将严重级别映射到输出流
        _severityToOutputStreamMap.emplace(severity, &(*it));
    }

    bool Logger::isSeverityActive(Severity severity) const
    {
        return _severityToOutputStreamMap.find(severity) != _severityToOutputStreamMap.end();
    }

    void Logger::processLog(const Log& log)
    {
        processLog(log.getModule(), log.getSeverity(), log.getMessage());
    }

    void Logger::processLog(Module module, Severity severity, std::string_view message)
    {
        auto it = _severityToOutputStreamMap.find(severity);
        if (it == _severityToOutputStreamMap.end())
            return;

        OutputStream* outputStream = it->second;
        std::lock_guard<std::mutex> lock(outputStream->mutex);

        // 获取当前时间
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        // 格式化时间（使用线程安全的localtime）
        std::tm tm_buf;
        std::tm* tm_info = std::localtime(&time_t);
        if (tm_info)
        {
            tm_buf = *tm_info;
            tm_info = &tm_buf;
        }
        
        std::ostringstream oss;
        if (tm_info)
        {
            oss << std::put_time(tm_info, "%Y-%m-%d %H:%M:%S");
        }
        else
        {
            oss << "1970-01-01 00:00:00";
        }
        oss << "." << std::setfill('0') << std::setw(3) << ms.count();
        
        // 格式化日志消息
        outputStream->stream 
            << "[" << oss.str() << "] "
            << "[" << lms::core::logging::getSeverityName(severity) << "] "
            << "[" << lms::core::logging::getModuleName(module) << "] "
            << message
            << std::endl;
    }
} // namespace lms::core::logging

