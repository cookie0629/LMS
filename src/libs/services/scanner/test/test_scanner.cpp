#include <cassert>
#include <iostream>
#include <filesystem>

#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "services/scanner/IScannerService.hpp"

int main()
{
    std::cout << "==========================================" << std::endl;
    std::cout << "Scanner Service 测试程序" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;

    try
    {
        // 测试 1: 初始化日志系统
        std::cout << "测试 1: 初始化日志系统..." << std::endl;
        auto logger = lms::core::logging::createLogger();
        lms::core::Service<lms::core::logging::ILogger>::assign(std::move(logger));
        std::cout << "  ✅ 日志系统初始化成功" << std::endl;
        std::cout << std::endl;

        // 测试 2: 创建数据库
        std::cout << "测试 2: 创建数据库..." << std::endl;
        std::filesystem::path dbPath = "test_scanner.db";
        if (std::filesystem::exists(dbPath))
        {
            std::filesystem::remove(dbPath);
        }

        auto db = lms::db::createDb(dbPath, 5);
        std::cout << "  ✅ 数据库创建成功: " << dbPath << std::endl;
        std::cout << std::endl;

        // 测试 3: 准备数据库表
        std::cout << "测试 3: 准备数据库表..." << std::endl;
        {
            auto& session = db->getTLSSession();
            session.prepareTablesIfNeeded();
        }
        std::cout << "  ✅ 数据库表准备成功" << std::endl;
        std::cout << std::endl;

        // 测试 4: 创建 Scanner Service
        std::cout << "测试 4: 创建 Scanner Service..." << std::endl;
        std::filesystem::path cachePath = "test_scanner_cache";
        if (std::filesystem::exists(cachePath))
        {
            std::filesystem::remove_all(cachePath);
        }
        std::filesystem::create_directories(cachePath);

        auto scannerService = lms::scanner::createScannerService(*db, cachePath);
        assert(scannerService && "Scanner Service 应该被创建");
        std::cout << "  ✅ Scanner Service 创建成功" << std::endl;
        std::cout << std::endl;

        // 测试 5: 获取初始状态
        std::cout << "测试 5: 获取初始状态..." << std::endl;
        auto status = scannerService->getStatus();
        assert(status.currentState == lms::scanner::IScannerService::State::NotScheduled && "初始状态应该是 NotScheduled");
        std::cout << "  ✅ 初始状态正确: NotScheduled" << std::endl;
        std::cout << std::endl;

        // 测试 6: 请求立即扫描（空媒体库）
        std::cout << "测试 6: 请求立即扫描（空媒体库）..." << std::endl;
        lms::scanner::ScanOptions options;
        scannerService->requestImmediateScan(options);
        std::cout << "  ✅ 扫描请求已提交" << std::endl;
        std::cout << std::endl;

        // 测试 7: 检查扫描状态
        std::cout << "测试 7: 检查扫描状态..." << std::endl;
        status = scannerService->getStatus();
        std::cout << "  当前状态: " << static_cast<int>(status.currentState) << std::endl;
        std::cout << "  (0=NotScheduled, 1=Scheduled, 2=InProgress)" << std::endl;
        std::cout << "  ✅ 状态检查完成" << std::endl;
        std::cout << std::endl;

        // 测试 8: 获取事件对象
        std::cout << "测试 8: 获取事件对象..." << std::endl;
        auto& events = scannerService->getEvents();
        (void)events; // 未使用
        std::cout << "  ✅ 事件对象获取成功" << std::endl;
        std::cout << std::endl;

        // 测试 9: 请求重新加载
        std::cout << "测试 9: 请求重新加载..." << std::endl;
        scannerService->requestReload();
        std::cout << "  ✅ 重新加载请求已提交" << std::endl;
        std::cout << std::endl;

        std::cout << "==========================================" << std::endl;
        std::cout << "所有测试通过！✅" << std::endl;
        std::cout << "==========================================" << std::endl;

        // 清理
        scannerService.reset();
        db.reset();
        if (std::filesystem::exists(dbPath))
        {
            std::filesystem::remove(dbPath);
        }
        if (std::filesystem::exists(cachePath))
        {
            std::filesystem::remove_all(cachePath);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "❌ 测试失败: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

