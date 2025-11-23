#include <iostream>
#include <filesystem>
#include <cassert>
#include <thread>
#include <chrono>

#include "core/ILogger.hpp"
#include "core/Service.hpp"
#include "database/IDb.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/User.hpp"
#include "services/auth/IPasswordService.hpp"
#include "services/auth/IAuthTokenService.hpp"
#include "services/auth/Types.hpp"
#include <boost/asio/ip/address.hpp>

int main()
{
    std::cout << "==========================================" << std::endl;
    std::cout << "Auth Service 认证服务测试" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;

    // 初始化日志系统
    auto logger = lms::core::logging::createLogger();
    lms::core::Service<lms::core::logging::ILogger>::assign(std::move(logger));
    
    try
    {
        // 测试 1: 创建数据库
        std::cout << "测试 1: 创建数据库..." << std::endl;
        std::filesystem::path testDbPath = "test_auth.db";
        
        // 如果测试数据库已存在，删除它
        if (std::filesystem::exists(testDbPath))
        {
            std::filesystem::remove(testDbPath);
            std::cout << "  删除旧的测试数据库文件" << std::endl;
        }
        
        auto db = lms::db::createDb(testDbPath, 5);
        std::cout << "  ✅ 数据库创建成功: " << testDbPath << std::endl;
        std::cout << std::endl;

        // 测试 2: 准备数据库表
        std::cout << "测试 2: 准备数据库表..." << std::endl;
        auto& session = db->getTLSSession();
        session.prepareTablesIfNeeded();
        session.migrateSchemaIfNeeded();
        std::cout << "  ✅ 数据库表准备完成" << std::endl;
        std::cout << std::endl;

        // 测试 3: 创建测试用户
        std::cout << "测试 3: 创建测试用户..." << std::endl;
        lms::db::UserId testUserId;
        {
            auto transaction = session.createWriteTransaction();
            auto user = session.create<lms::db::User>("testuser");
            testUserId = user->getId();
            transaction.commit();  // 显式提交事务
            std::cout << "  ✅ 用户创建成功: " << user->getLoginName() << " (ID: " << testUserId.getValue() << ")" << std::endl;
        }
        std::cout << std::endl;

        // 测试 4: 创建 PasswordService
        std::cout << "测试 4: 创建 PasswordService..." << std::endl;
        auto passwordService = lms::auth::createPasswordService("internal", *db, 1000);
        std::cout << "  ✅ PasswordService 创建成功" << std::endl;
        std::cout << std::endl;

        // 测试 5: 测试密码设置和验证
        std::cout << "测试 5: 测试密码设置和验证..." << std::endl;
        {
            // 设置密码（setPassword 内部会创建事务）
            std::string password = "TestPassword123!";
            passwordService->setPassword(testUserId, password);
            std::cout << "  ✅ 密码设置成功" << std::endl;

            // 验证密码（在事务外，因为 checkUserPassword 内部会创建事务）
            boost::asio::ip::address clientAddress = boost::asio::ip::address::from_string("127.0.0.1");
            auto checkResult = passwordService->checkUserPassword(clientAddress, "testuser", "TestPassword123!");
            assert(checkResult.state == lms::auth::IPasswordService::CheckResult::State::Granted && "密码验证应该成功");
            std::cout << "  ✅ 密码验证成功" << std::endl;

            // 验证错误密码
            auto invalidResult = passwordService->checkUserPassword(clientAddress, "testuser", "WrongPassword");
            assert(invalidResult.state != lms::auth::IPasswordService::CheckResult::State::Granted && "错误密码应该验证失败");
            std::cout << "  ✅ 错误密码验证失败（符合预期）" << std::endl;
        }
        std::cout << std::endl;

        // 测试 6: 测试密码强度检查
        std::cout << "测试 6: 测试密码强度检查..." << std::endl;
        {
            // 获取用户信息（在事务内）
            std::string loginName;
            lms::db::UserType userType;
            {
                auto transaction = session.createReadTransaction();
                auto user = lms::db::User::find(session, testUserId);
                assert(user && "用户应该存在");
                loginName = user->getLoginName();
                userType = user->getType();
            }

            // 测试弱密码（在事务外）
            lms::auth::PasswordValidationContext weakContext;
            weakContext.loginName = loginName;
            weakContext.userType = userType;
            auto weakResult = passwordService->checkPasswordAcceptability("weak", weakContext);
            assert(weakResult != lms::auth::IPasswordService::PasswordAcceptabilityResult::OK && "弱密码应该被拒绝");
            std::cout << "  ✅ 弱密码被拒绝（符合预期）" << std::endl;

            // 测试强密码
            lms::auth::PasswordValidationContext strongContext;
            strongContext.loginName = loginName;
            strongContext.userType = userType;
            auto strongResult = passwordService->checkPasswordAcceptability("StrongPassword123!", strongContext);
            assert(strongResult == lms::auth::IPasswordService::PasswordAcceptabilityResult::OK && "强密码应该被接受");
            std::cout << "  ✅ 强密码被接受" << std::endl;
        }
        std::cout << std::endl;

        // 测试 7: 创建 AuthTokenService
        std::cout << "测试 7: 创建 AuthTokenService..." << std::endl;
        auto authTokenService = lms::auth::createAuthTokenService(*db, 1000);
        std::cout << "  ✅ AuthTokenService 创建成功" << std::endl;
        std::cout << std::endl;

        // 测试 8: 注册域
        std::cout << "测试 8: 注册域..." << std::endl;
        lms::auth::IAuthTokenService::DomainParameters domainParams;
        domainParams.tokenDuration = std::chrono::hours(24);
        domainParams.tokenMaxUseCount = 10;
        authTokenService->registerDomain("test", domainParams);
        std::cout << "  ✅ 域注册成功" << std::endl;
        std::cout << std::endl;

        // 测试 9: 创建认证令牌
        std::cout << "测试 9: 创建认证令牌..." << std::endl;
        std::string tokenValue = "test-token-12345";
        authTokenService->createAuthToken("test", testUserId, tokenValue);
        std::cout << "  ✅ 认证令牌创建成功: " << tokenValue << std::endl;
        std::cout << std::endl;

        // 测试 10: 验证认证令牌
        std::cout << "测试 10: 验证认证令牌..." << std::endl;
        {
            boost::asio::ip::address clientAddress = boost::asio::ip::address::from_string("127.0.0.1");
            auto result = authTokenService->processAuthToken("test", clientAddress, tokenValue);
            if (result.state != lms::auth::IAuthTokenService::AuthTokenProcessResult::State::Granted)
            {
                std::cout << "  ❌ 令牌验证失败，状态: " << static_cast<int>(result.state) << std::endl;
                std::cout << "     (0=Granted, 1=Throttled, 2=Denied)" << std::endl;
            }
            assert(result.state == lms::auth::IAuthTokenService::AuthTokenProcessResult::State::Granted && "令牌应该被授权");
            assert(result.authTokenInfo.has_value() && "应该有令牌信息");
            assert(result.authTokenInfo->userId == testUserId && "用户ID应该匹配");
            std::cout << "  ✅ 认证令牌验证成功" << std::endl;
            std::cout << "     用户ID: " << result.authTokenInfo->userId.getValue() << std::endl;
            std::cout << "     使用次数: " << result.authTokenInfo->useCount << std::endl;
        }
        std::cout << std::endl;

        // 测试 11: 访问用户的所有令牌
        std::cout << "测试 11: 访问用户的所有令牌..." << std::endl;
        {
            int tokenCount = 0;
            authTokenService->visitAuthTokens("test", testUserId, [&tokenCount](const lms::auth::IAuthTokenService::AuthTokenInfo&, std::string_view token) {
                tokenCount++;
                std::cout << "     令牌 " << tokenCount << ": " << token << std::endl;
            });
            assert(tokenCount > 0 && "应该至少有一个令牌");
            std::cout << "  ✅ 找到 " << tokenCount << " 个令牌" << std::endl;
        }
        std::cout << std::endl;

        // 测试 12: 测试登录限流
        std::cout << "测试 12: 测试登录限流..." << std::endl;
        {
            boost::asio::ip::address clientAddress = boost::asio::ip::address::from_string("192.168.1.100");
            
            // 尝试多次失败登录
            for (int i = 0; i < 6; ++i)
            {
                auto result = authTokenService->processAuthToken("test", clientAddress, "invalid-token");
                assert(result.state == lms::auth::IAuthTokenService::AuthTokenProcessResult::State::Denied && "无效令牌应该被拒绝");
            }
            std::cout << "  ✅ 6 次失败登录完成" << std::endl;

            // 验证是否被限流
            auto throttledResult = authTokenService->processAuthToken("test", clientAddress, tokenValue);
            assert(throttledResult.state == lms::auth::IAuthTokenService::AuthTokenProcessResult::State::Throttled && "应该被限流");
            std::cout << "  ✅ 客户端被限流（符合预期）" << std::endl;
        }
        std::cout << std::endl;

        // 测试 13: 清除用户令牌
        std::cout << "测试 13: 清除用户令牌..." << std::endl;
        {
            authTokenService->clearAuthTokens("test", testUserId);
            std::cout << "  ✅ 用户令牌清除成功" << std::endl;

            // 验证令牌已被清除
            int tokenCount = 0;
            authTokenService->visitAuthTokens("test", testUserId, [&tokenCount](const lms::auth::IAuthTokenService::AuthTokenInfo&, std::string_view) {
                tokenCount++;
            });
            assert(tokenCount == 0 && "令牌应该已被清除");
            std::cout << "  ✅ 验证令牌已被清除" << std::endl;
        }
        std::cout << std::endl;

        std::cout << "==========================================" << std::endl;
        std::cout << "✅ 所有测试通过！" << std::endl;
        std::cout << "==========================================" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "❌ 测试失败: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

