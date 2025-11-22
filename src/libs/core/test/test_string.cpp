#include <iostream>
#include <vector>
#include "core/String.hpp"

int main()
{
    std::cout << "==========================================" << std::endl;
    std::cout << "String 工具类测试" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;

    // 测试 splitString (char)
    std::cout << "测试 splitString (char):" << std::endl;
    std::string testStr = "apple,banana,cherry";
    auto parts = lms::core::stringUtils::splitString(testStr, ',');
    std::cout << "  原始: " << testStr << std::endl;
    std::cout << "  分割结果: ";
    for (size_t i = 0; i < parts.size(); ++i)
    {
        std::cout << "'" << parts[i] << "'";
        if (i < parts.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl << std::endl;

    // 测试 splitString (string)
    std::cout << "测试 splitString (string):" << std::endl;
    std::string testStr2 = "hello::world::test";
    auto parts2 = lms::core::stringUtils::splitString(testStr2, "::");
    std::cout << "  原始: " << testStr2 << std::endl;
    std::cout << "  分割结果: ";
    for (size_t i = 0; i < parts2.size(); ++i)
    {
        std::cout << "'" << parts2[i] << "'";
        if (i < parts2.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl << std::endl;

    // 测试 joinStrings
    std::cout << "测试 joinStrings:" << std::endl;
    std::vector<std::string> words = {"apple", "banana", "cherry"};
    std::string joined = lms::core::stringUtils::joinStrings(words, ", ");
    std::cout << "  连接结果: " << joined << std::endl;
    std::cout << std::endl;

    // 测试 stringTrim
    std::cout << "测试 stringTrim:" << std::endl;
    std::string_view trimmed = lms::core::stringUtils::stringTrim("  hello world  ");
    std::cout << "  原始: '  hello world  '" << std::endl;
    std::cout << "  修剪后: '" << trimmed << "'" << std::endl;
    std::cout << std::endl;

    // 测试 stringToLower
    std::cout << "测试 stringToLower:" << std::endl;
    std::string lower = lms::core::stringUtils::stringToLower("Hello World");
    std::cout << "  原始: Hello World" << std::endl;
    std::cout << "  小写: " << lower << std::endl;
    std::cout << std::endl;

    // 测试 stringToUpper
    std::cout << "测试 stringToUpper:" << std::endl;
    std::string upper = lms::core::stringUtils::stringToUpper("Hello World");
    std::cout << "  原始: Hello World" << std::endl;
    std::cout << "  大写: " << upper << std::endl;
    std::cout << std::endl;

    // 测试 stringCaseInsensitiveEqual
    std::cout << "测试 stringCaseInsensitiveEqual:" << std::endl;
    bool equal1 = lms::core::stringUtils::stringCaseInsensitiveEqual("Hello", "hello");
    bool equal2 = lms::core::stringUtils::stringCaseInsensitiveEqual("Hello", "world");
    std::cout << "  'Hello' == 'hello': " << (equal1 ? "true" : "false") << std::endl;
    std::cout << "  'Hello' == 'world': " << (equal2 ? "true" : "false") << std::endl;
    std::cout << std::endl;

    // 测试 stringCaseInsensitiveContains
    std::cout << "测试 stringCaseInsensitiveContains:" << std::endl;
    bool contains1 = lms::core::stringUtils::stringCaseInsensitiveContains("Hello World", "world");
    bool contains2 = lms::core::stringUtils::stringCaseInsensitiveContains("Hello World", "test");
    std::cout << "  'Hello World' 包含 'world': " << (contains1 ? "true" : "false") << std::endl;
    std::cout << "  'Hello World' 包含 'test': " << (contains2 ? "true" : "false") << std::endl;
    std::cout << std::endl;

    // 测试 capitalize
    std::cout << "测试 capitalize:" << std::endl;
    std::string capStr = "hello world";
    lms::core::stringUtils::capitalize(capStr);
    std::cout << "  原始: hello world" << std::endl;
    std::cout << "  首字母大写: " << capStr << std::endl;
    std::cout << std::endl;

    std::cout << "==========================================" << std::endl;
    std::cout << "测试完成！" << std::endl;
    std::cout << "==========================================" << std::endl;

    return 0;
}

