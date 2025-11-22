#pragma once

#include "core/IConfig.hpp"

#include <libconfig.h++>
#include <filesystem>

namespace lms::core
{
    /**
     * @brief 配置实现类，使用 libconfig++ 解析配置文件
     */
    class Config final : public IConfig
    {
    public:
        /**
         * @brief 构造函数
         * @param configPath 配置文件路径
         * @throws std::runtime_error 如果配置文件无法读取
         */
        explicit Config(const std::filesystem::path& configPath);

        ~Config() override = default;

        // 禁止拷贝和移动
        Config(const Config&) = delete;
        Config& operator=(const Config&) = delete;
        Config(Config&&) = delete;
        Config& operator=(Config&&) = delete;

    private:
        // IConfig 接口实现
        std::string_view getString(std::string_view setting, std::string_view def) override;
        void visitStrings(std::string_view setting, 
                         std::function<void(std::string_view)> func, 
                         std::initializer_list<std::string_view> def) override;
        std::filesystem::path getPath(std::string_view setting, const std::filesystem::path& def) override;
        unsigned long getULong(std::string_view setting, unsigned long def) override;
        long getLong(std::string_view setting, long def) override;
        bool getBool(std::string_view setting, bool def) override;

        libconfig::Config _config;
        std::string _configFilePath;  // 保存配置文件路径用于错误信息
    };
} // namespace lms::core

