#include "Config.hpp"

#include <filesystem>
#include <stdexcept>
#include <sstream>
#include <string>

namespace lms::core
{
    Config::Config(const std::filesystem::path& configPath)
        : _configFilePath(configPath.string())
    {
        try
        {
            _config.readFile(configPath.string().c_str());
        }
        catch (const libconfig::FileIOException& e)
        {
            throw std::runtime_error("无法读取配置文件: " + _configFilePath + " - " + e.what());
        }
        catch (const libconfig::ParseException& e)
        {
            std::ostringstream oss;
            oss << "配置文件解析错误: " << e.getFile() << ":" << e.getLine() << " - " << e.getError();
            throw std::runtime_error(oss.str());
        }
    }

    std::string_view Config::getString(std::string_view setting, std::string_view def)
    {
        try
        {
            return static_cast<const char*>(_config.lookup(std::string{ setting }));
        }
        catch (const libconfig::ConfigException&)
        {
            return def;
        }
    }

    void Config::visitStrings(std::string_view setting, 
                              std::function<void(std::string_view)> func, 
                              std::initializer_list<std::string_view> def)
    {
        try
        {
            const libconfig::Setting& values = _config.lookup(std::string{ setting });
            for (int i = 0; i < values.getLength(); ++i)
            {
                func(static_cast<const char*>(values[i]));
            }
        }
        catch (const libconfig::SettingNotFoundException&)
        {
            // 配置项不存在，使用默认值
            for (std::string_view defValue : def)
            {
                func(defValue);
            }
        }
        catch (const libconfig::ConfigException&)
        {
            // 其他配置异常，忽略
        }
    }

    std::filesystem::path Config::getPath(std::string_view setting, const std::filesystem::path& def)
    {
        try
        {
            const char* res = _config.lookup(std::string{ setting });
            return std::filesystem::path{ std::string(res) };
        }
        catch (const libconfig::ConfigException&)
        {
            return def;
        }
    }

    unsigned long Config::getULong(std::string_view setting, unsigned long def)
    {
        try
        {
            return static_cast<unsigned long>(_config.lookup(std::string{ setting }));
        }
        catch (const libconfig::ConfigException&)
        {
            return def;
        }
    }

    long Config::getLong(std::string_view setting, long def)
    {
        try
        {
            return _config.lookup(std::string{ setting });
        }
        catch (const libconfig::ConfigException&)
        {
            return def;
        }
    }

    bool Config::getBool(std::string_view setting, bool def)
    {
        try
        {
            return _config.lookup(std::string{ setting });
        }
        catch (const libconfig::ConfigException&)
        {
            return def;
        }
    }

    // 工厂函数
    std::unique_ptr<IConfig> createConfig(const std::filesystem::path& configPath)
    {
        return std::make_unique<Config>(configPath);
    }
} // namespace lms::core

