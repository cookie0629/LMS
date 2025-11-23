#pragma once

#include <filesystem>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>

#include "core/Service.hpp"

namespace lms::core::logging
{
    /**
     * @brief 日志严重级别
     */
    enum class Severity
    {
        FATAL,    // 致命错误
        ERROR,    // 错误
        WARNING,  // 警告
        INFO,     // 信息
        DEBUG_LEVEL,    // 调试（使用DEBUG_LEVEL避免与DEBUG宏冲突）
    };

    /**
     * @brief 日志模块
     */
    enum class Module
    {
        API_SUBSONIC,
        AUDIO,
        AUTH,
        CHILDPROCESS,
        COVER,
        DB,
        DBUPDATER,
        FEATURE,
        FEEDBACK,
        HTTP,
        MAIN,
        METADATA,
        PODCAST,
        REMOTE,
        SCANNER,
        SCROBBLING,
        SERVICE,
        RECOMMENDATION,
        TRANSCODING,
        UI,
        UTILS,
        WT,
    };

    /**
     * @brief 获取模块名称
     * @param mod 模块枚举
     * @return 模块名称字符串
     */
    const char* getModuleName(Module mod);

    /**
     * @brief 获取严重级别名称
     * @param sev 严重级别枚举
     * @return 严重级别名称字符串
     */
    const char* getSeverityName(Severity sev);

    /**
     * @brief 日志接口
     */
    class ILogger;

    /**
     * @brief 日志对象，用于构建日志消息
     */
    class Log
    {
    public:
        /**
         * @brief 构造函数
         * @param logger 日志器引用
         * @param module 模块
         * @param severity 严重级别
         */
        Log(ILogger& logger, Module module, Severity severity);

        /**
         * @brief 析构函数，自动输出日志
         */
        ~Log();

        /**
         * @brief 获取模块
         */
        Module getModule() const { return _module; }

        /**
         * @brief 获取严重级别
         */
        Severity getSeverity() const { return _severity; }

        /**
         * @brief 获取日志消息
         */
        std::string getMessage() const;

        /**
         * @brief 获取输出流，用于构建日志消息
         */
        std::ostringstream& getOstream() { return _oss; }

    private:
        Log(const Log&) = delete;
        Log& operator=(const Log&) = delete;

        ILogger& _logger;
        Module _module;
        Severity _severity;
        std::ostringstream _oss;
    };

    /**
     * @brief 日志器接口
     */
    class ILogger
    {
    public:
        virtual ~ILogger() = default;

        /**
         * @brief 检查严重级别是否激活
         * @param severity 严重级别
         * @return true 如果该级别会输出，false 否则
         */
        virtual bool isSeverityActive(Severity severity) const = 0;

        /**
         * @brief 处理日志（通过 Log 对象）
         * @param log 日志对象
         */
        virtual void processLog(const Log& log) = 0;

        /**
         * @brief 处理日志（直接消息）
         * @param module 模块
         * @param severity 严重级别
         * @param message 消息
         */
        virtual void processLog(Module module, Severity severity, std::string_view message) = 0;
    };

    /**
     * @brief 默认最小严重级别
     */
    static constexpr Severity defaultMinSeverity{ Severity::INFO };

    /**
     * @brief 创建日志器
     * @param minSeverity 最小严重级别
     * @param logFilePath 日志文件路径（空表示输出到控制台）
     * @return 日志器指针
     */
    std::unique_ptr<ILogger> createLogger(Severity minSeverity = defaultMinSeverity, 
                                          const std::filesystem::path& logFilePath = {});
} // namespace lms::core::logging

/**
 * @brief 日志宏
 * 
 * 使用示例：
 * LMS_LOG(MAIN, INFO, "服务器启动");
 * LMS_LOG(MAIN, DEBUG, "变量值: " << variable);
 */
// 如果DEBUG宏已定义，先取消定义
#ifdef DEBUG
#undef DEBUG
#endif

#define LMS_LOG(module, severity, message)                                                                                                                               \
    do                                                                                                                                                                   \
    {                                                                                                                                                                    \
        if (auto* logger_{ ::lms::core::Service<::lms::core::logging::ILogger>::get() }; logger_ && logger_->isSeverityActive(::lms::core::logging::Severity::severity)) \
            ::lms::core::logging::Log{ *logger_, ::lms::core::logging::Module::module, ::lms::core::logging::Severity::severity }.getOstream() << message;               \
    } while (0)

// DEBUG 宏的别名（因为DEBUG是预定义宏，用于日志宏中）
#define DEBUG DEBUG_LEVEL

/**
 * @brief 条件日志宏
 * 
 * 使用示例：
 * LMS_LOG_IF(MAIN, DEBUG, condition, "条件满足");
 */
#define LMS_LOG_IF(module, severity, cond, message)                                                                                                                              \
    do                                                                                                                                                                           \
    {                                                                                                                                                                            \
        if (auto* logger_{ ::lms::core::Service<::lms::core::logging::ILogger>::get() }; logger_ && logger_->isSeverityActive(::lms::core::logging::Severity::severity) && cond) \
            ::lms::core::logging::Log{ *logger_, ::lms::core::logging::Module::module, ::lms::core::logging::Severity::severity }.getOstream() << message;                       \
    } while (0)

