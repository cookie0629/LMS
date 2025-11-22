#include <iostream>
#include <filesystem>
#include "core/ILogger.hpp"
#include "core/Service.hpp"

int main()
{
    std::cout << "==========================================" << std::endl;
    std::cout << "Logger 日志系统测试" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;

    try
    {
        // 创建日志器（输出到控制台，最小级别为 DEBUG）
        auto logger = lms::core::logging::createLogger(
            lms::core::logging::Severity::DEBUG_LEVEL,
            {}  // 空路径表示输出到控制台
        );

        // 注册到 Service
        lms::core::Service<lms::core::logging::ILogger>::assign(std::move(logger));

        std::cout << "✅ 日志器创建并注册成功" << std::endl;
        std::cout << std::endl;

        // 测试不同级别的日志
        std::cout << "测试不同级别的日志:" << std::endl;
        std::cout << "---" << std::endl;

        LMS_LOG(MAIN, FATAL, "这是一条 FATAL 日志");
        LMS_LOG(MAIN, ERROR, "这是一条 ERROR 日志");
        LMS_LOG(MAIN, WARNING, "这是一条 WARNING 日志");
        LMS_LOG(MAIN, INFO, "这是一条 INFO 日志");
        LMS_LOG(MAIN, DEBUG, "这是一条 DEBUG 日志");

        std::cout << "---" << std::endl;
        std::cout << std::endl;

        // 测试带变量的日志
        std::cout << "测试带变量的日志:" << std::endl;
        std::cout << "---" << std::endl;

        int port = 5082;
        std::string serverName = "LMS";
        LMS_LOG(MAIN, INFO, "服务器 " << serverName << " 启动在端口 " << port);

        std::cout << "---" << std::endl;
        std::cout << std::endl;

        // 测试条件日志
        std::cout << "测试条件日志:" << std::endl;
        std::cout << "---" << std::endl;

        bool debugMode = true;
        LMS_LOG_IF(MAIN, DEBUG, debugMode, "调试模式已启用");

        debugMode = false;
        LMS_LOG_IF(MAIN, DEBUG, debugMode, "这条日志不会输出");

        std::cout << "---" << std::endl;
        std::cout << std::endl;

        // 测试不同模块
        std::cout << "测试不同模块的日志:" << std::endl;
        std::cout << "---" << std::endl;

        LMS_LOG(AUDIO, INFO, "音频文件处理完成");
        LMS_LOG(DB, INFO, "数据库连接成功");
        LMS_LOG(UI, INFO, "Web界面初始化");

        std::cout << "---" << std::endl;
        std::cout << std::endl;

        std::cout << "==========================================" << std::endl;
        std::cout << "测试完成！" << std::endl;
        std::cout << "==========================================" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

