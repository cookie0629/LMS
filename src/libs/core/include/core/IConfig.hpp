#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <string_view>

namespace lms::core
{
    /**
     * @brief 配置接口，用于从配置文件读取配置值
     */
    class IConfig
    {
    public:
        virtual ~IConfig() = default;

        /**
         * @brief 获取字符串配置值
         * @param setting 配置项名称
         * @param def 默认值（如果配置项不存在）
         * @return 配置值或默认值
         */
        virtual std::string_view getString(std::string_view setting, std::string_view def) = 0;

        /**
         * @brief 访问多值字符串配置（如数组）
         * @param setting 配置项名称
         * @param func 对每个值调用的函数
         * @param def 默认值列表（如果配置项不存在）
         */
        virtual void visitStrings(std::string_view setting, 
                                  std::function<void(std::string_view)> func, 
                                  std::initializer_list<std::string_view> def) = 0;

        /**
         * @brief 获取路径配置值
         * @param setting 配置项名称
         * @param def 默认路径
         * @return 配置路径或默认路径
         */
        virtual std::filesystem::path getPath(std::string_view setting, const std::filesystem::path& def) = 0;

        /**
         * @brief 获取无符号长整数配置值
         * @param setting 配置项名称
         * @param def 默认值
         * @return 配置值或默认值
         */
        virtual unsigned long getULong(std::string_view setting, unsigned long def) = 0;

        /**
         * @brief 获取长整数配置值
         * @param setting 配置项名称
         * @param def 默认值
         * @return 配置值或默认值
         */
        virtual long getLong(std::string_view setting, long def) = 0;

        /**
         * @brief 获取布尔配置值
         * @param setting 配置项名称
         * @param def 默认值
         * @return 配置值或默认值
         */
        virtual bool getBool(std::string_view setting, bool def) = 0;
    };

    /**
     * @brief 创建配置对象
     * @param configPath 配置文件路径
     * @return 配置对象指针
     */
    std::unique_ptr<IConfig> createConfig(const std::filesystem::path& configPath);
} // namespace lms::core

