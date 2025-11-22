#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <span>

namespace lms::core::stringUtils
{
    /**
     * @brief 分割字符串
     * @param str 要分割的字符串
     * @param separator 分隔符
     * @return 分割后的字符串视图向量
     */
    std::vector<std::string_view> splitString(std::string_view str, char separator);

    /**
     * @brief 分割字符串（使用字符串分隔符）
     * @param str 要分割的字符串
     * @param separator 分隔符字符串
     * @return 分割后的字符串视图向量
     */
    std::vector<std::string_view> splitString(std::string_view str, std::string_view separator);

    /**
     * @brief 连接字符串
     * @param strings 字符串向量
     * @param delimiter 分隔符
     * @return 连接后的字符串
     */
    std::string joinStrings(std::span<const std::string> strings, std::string_view delimiter);

    /**
     * @brief 连接字符串（使用字符分隔符）
     * @param strings 字符串向量
     * @param delimiter 分隔符字符
     * @return 连接后的字符串
     */
    std::string joinStrings(std::span<const std::string> strings, char delimiter);

    /**
     * @brief 去除字符串两端的空白字符
     * @param str 要处理的字符串
     * @param whitespaces 空白字符集合（默认为空格、制表符、回车符）
     * @return 处理后的字符串视图
     */
    std::string_view stringTrim(std::string_view str, std::string_view whitespaces = " \t\r\n");

    /**
     * @brief 去除字符串左端的空白字符
     * @param str 要处理的字符串
     * @param whitespaces 空白字符集合
     * @return 处理后的字符串视图
     */
    std::string_view stringTrimStart(std::string_view str, std::string_view whitespaces = " \t\r\n");

    /**
     * @brief 去除字符串右端的空白字符
     * @param str 要处理的字符串
     * @param whitespaces 空白字符集合
     * @return 处理后的字符串视图
     */
    std::string_view stringTrimEnd(std::string_view str, std::string_view whitespaces = " \t\r\n");

    /**
     * @brief 将字符串转换为小写
     * @param str 要转换的字符串
     * @return 转换后的小写字符串
     */
    std::string stringToLower(std::string_view str);

    /**
     * @brief 将字符串转换为小写（原地修改）
     * @param str 要转换的字符串（会被修改）
     */
    void stringToLower(std::string& str);

    /**
     * @brief 将字符串转换为大写
     * @param str 要转换的字符串
     * @return 转换后的大写字符串
     */
    std::string stringToUpper(std::string_view str);

    /**
     * @brief 将字符串转换为大写（原地修改）
     * @param str 要转换的字符串（会被修改）
     */
    void stringToUpper(std::string& str);

    /**
     * @brief 不区分大小写的字符串比较
     * @param strA 第一个字符串
     * @param strB 第二个字符串
     * @return true 如果字符串相等（忽略大小写），false 否则
     */
    bool stringCaseInsensitiveEqual(std::string_view strA, std::string_view strB);

    /**
     * @brief 检查字符串是否包含子字符串（不区分大小写）
     * @param str 要搜索的字符串
     * @param substr 要查找的子字符串
     * @return true 如果包含，false 否则
     */
    bool stringCaseInsensitiveContains(std::string_view str, std::string_view substr);

    /**
     * @brief 首字母大写
     * @param str 要处理的字符串（会被修改）
     */
    void capitalize(std::string& str);
} // namespace lms::core::stringUtils

