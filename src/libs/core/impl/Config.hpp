
#pragma once

#include "core/IConfig.hpp"

#include <libconfig.h++>

namespace lms::core
{
    // Config: конкретная реализация IConfig, использующая libconfig++ для чтения файла.
    // Config：IConfig 的具体实现，内部使用 libconfig++ 读取配置文件。
    class Config final : public IConfig
    {
    public:
        // p: путь к конфигурационному файлу (например conf/lms.conf)。
        // p：配置文件路径（例如 conf/lms.conf）。
        Config(const std::filesystem::path& p);
        ~Config() override = default;

        Config(const Config&) = delete;
        Config& operator=(const Config&) = delete;
        Config(Config&&) = delete;
        Config& operator=(Config&&) = delete;

    private:
        // Default values are returned in case of setting not found
        std::string_view getString(std::string_view setting, std::string_view def) override;
        void visitStrings(std::string_view setting, std::function<void(std::string_view)> _func, std::initializer_list<std::string_view> defs) override;
        std::filesystem::path getPath(std::string_view setting, const std::filesystem::path& def) override;
        unsigned long getULong(std::string_view setting, unsigned long def) override;
        long getLong(std::string_view setting, long def) override;
        bool getBool(std::string_view setting, bool def) override;

        libconfig::Config _config;
    };
} // namespace lms::core