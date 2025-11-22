#include <iostream>
#include <vector>
#include "core/UUID.hpp"

int main()
{
    std::cout << "==========================================" << std::endl;
    std::cout << "UUID 工具类测试" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;

    // 测试 generate()
    std::cout << "测试 generate():" << std::endl;
    std::vector<lms::core::UUID> uuids;
    for (int i = 0; i < 5; ++i)
    {
        auto uuid = lms::core::UUID::generate();
        uuids.push_back(uuid);
        std::cout << "  UUID " << (i + 1) << ": " << uuid.getAsString() << std::endl;
    }
    std::cout << std::endl;

    // 测试唯一性
    std::cout << "测试唯一性:" << std::endl;
    bool allUnique = true;
    for (size_t i = 0; i < uuids.size(); ++i)
    {
        for (size_t j = i + 1; j < uuids.size(); ++j)
        {
            if (uuids[i] == uuids[j])
            {
                allUnique = false;
                std::cout << "  错误：发现重复的 UUID！" << std::endl;
            }
        }
    }
    if (allUnique)
    {
        std::cout << "  ✅ 所有 UUID 都是唯一的" << std::endl;
    }
    std::cout << std::endl;

    // 测试 fromString() - 有效格式
    std::cout << "测试 fromString() - 有效格式:" << std::endl;
    std::string validUUID = "123e4567-e89b-12d3-a456-426614174000";
    auto uuid1 = lms::core::UUID::fromString(validUUID);
    if (uuid1.has_value())
    {
        std::cout << "  输入: " << validUUID << std::endl;
        std::cout << "  解析成功: " << uuid1->getAsString() << std::endl;
    }
    else
    {
        std::cout << "  错误：无法解析有效的 UUID" << std::endl;
    }
    std::cout << std::endl;

    // 测试 fromString() - 无效格式
    std::cout << "测试 fromString() - 无效格式:" << std::endl;
    std::vector<std::string> invalidUUIDs = {
        "not-a-uuid",
        "123e4567-e89b-12d3-a456",  // 太短
        "123e4567-e89b-12d3-a456-426614174000-extra",  // 太长
        "123e4567e89b12d3a456426614174000",  // 没有连字符
    };
    
    for (const auto& invalid : invalidUUIDs)
    {
        auto uuid = lms::core::UUID::fromString(invalid);
        if (!uuid.has_value())
        {
            std::cout << "  ✅ 正确拒绝无效格式: " << invalid << std::endl;
        }
        else
        {
            std::cout << "  ❌ 错误接受无效格式: " << invalid << std::endl;
        }
    }
    std::cout << std::endl;

    // 测试比较操作符
    std::cout << "测试比较操作符:" << std::endl;
    auto uuid2 = lms::core::UUID::generate();
    auto uuid3 = lms::core::UUID::generate();
    auto uuid4 = uuid2;  // 复制
    
    std::cout << "  uuid2 == uuid3: " << (uuid2 == uuid3 ? "true" : "false") << std::endl;
    std::cout << "  uuid2 == uuid4: " << (uuid2 == uuid4 ? "true" : "false") << std::endl;
    std::cout << std::endl;

    std::cout << "==========================================" << std::endl;
    std::cout << "测试完成！" << std::endl;
    std::cout << "==========================================" << std::endl;

    return 0;
}

