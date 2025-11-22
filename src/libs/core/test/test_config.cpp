#include <iostream>
#include <filesystem>
#include "core/IConfig.hpp"

int main()
{
    std::cout << "==========================================" << std::endl;
    std::cout << "Config 配置系统测试" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;

    // 检查配置文件是否存在
    std::filesystem::path configPath = "conf/lms.conf.example";
    if (!std::filesystem::exists(configPath))
    {
        std::cout << "错误: 配置文件不存在: " << configPath << std::endl;
        std::cout << "请先创建配置文件" << std::endl;
        return 1;
    }

    try
    {
        // 创建配置对象
        auto config = lms::core::createConfig(configPath);
        std::cout << "✅ 配置文件加载成功" << std::endl;
        std::cout << std::endl;

        // 测试 getString()
        std::cout << "测试 getString():" << std::endl;
        std::string_view logLevel = config->getString("log-min-severity", "info");
        std::cout << "  log-min-severity: " << logLevel << std::endl;
        std::cout << std::endl;

        // 测试 getULong()
        std::cout << "测试 getULong():" << std::endl;
        unsigned long port = config->getULong("listen-port", 5082);
        std::cout << "  listen-port: " << port << std::endl;
        std::cout << std::endl;

        // 测试 getBool()
        std::cout << "测试 getBool():" << std::endl;
        bool tlsEnabled = config->getBool("tls-enable", false);
        std::cout << "  tls-enable: " << (tlsEnabled ? "true" : "false") << std::endl;
        std::cout << std::endl;

        // 测试 getPath()
        std::cout << "测试 getPath():" << std::endl;
        std::filesystem::path workDir = config->getPath("working-dir", std::filesystem::path("/var/lms"));
        std::cout << "  working-dir: " << workDir << std::endl;
        std::cout << std::endl;

        // 测试 visitStrings()
        std::cout << "测试 visitStrings():" << std::endl;
        std::cout << "  trusted-proxies: ";
        config->visitStrings("trusted-proxies", 
            [](std::string_view value) {
                std::cout << value << " ";
            },
            {"127.0.0.1", "::1"});
        std::cout << std::endl << std::endl;

        // 测试默认值
        std::cout << "测试默认值:" << std::endl;
        std::string_view notExist = config->getString("non-existent-setting", "default-value");
        std::cout << "  non-existent-setting: " << notExist << " (使用默认值)" << std::endl;
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

