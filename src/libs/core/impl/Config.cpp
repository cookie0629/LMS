
#include "Config.hpp"

#include "core/Exception.hpp"

namespace lms::core
{
    // createConfig: фабричная функция, скрывающая конкретный тип Config за интерфейсом IConfig。
    // createConfig：工厂函数，通过 IConfig 接口隐藏具体的 Config 类型。
    std::unique_ptr<IConfig> createConfig(const std::filesystem::path& p)
    {
        return std::make_unique<Config>(p);
    }

    // Конструктор Config: читает конфигурационный файл и поднимает исключения, если что‑то пошло не так。
    // Config 构造函数：读取配置文件，如果出错则抛出异常。
    Config::Config(const std::filesystem::path& p)
    {
        try
        {
            _config.readFile(p.c_str());
        }
        catch (libconfig::FileIOException& e)
        {
            throw LmsException{ "Cannot open config file '" + p.string() + "'" };
        }
        catch (libconfig::ParseException& e)
        {
            throw LmsException{ "Cannot parse config file '" + p.string() + "', line = " + std::to_string(e.getLine()) + ", error = '" + e.getError() + "'" };
        }
        catch (libconfig::ConfigException& e)
        {
            throw LmsException{ "Cannot open config file '" + p.string() + "': " + e.what() };
        }
    }

    std::string_view Config::getString(std::string_view setting, std::string_view def)
    {
        try
        {
            return static_cast<const char*>(_config.lookup(std::string{ setting }));
        }
        catch (libconfig::ConfigException&)
        {
            return def;
        }
    }

    void Config::visitStrings(std::string_view setting, std::function<void(std::string_view)> _func, std::initializer_list<std::string_view> defs)
    {
        try
        {
            const libconfig::Setting& values{ _config.lookup(std::string{ setting }) };
            for (int i{}; i < values.getLength(); ++i)
                _func(static_cast<const char*>(values[i]));
        }
        catch (const libconfig::SettingNotFoundException&)
        {
            for (std::string_view def : defs)
                _func(def);
        }
        catch (libconfig::ConfigException&)
        {
        }
    }

    std::filesystem::path Config::getPath(std::string_view setting, const std::filesystem::path& path)
    {
        try
        {
            const char* res{ _config.lookup(std::string{ setting }) };
            return std::filesystem::path{ std::string(res) };
        }
        catch (libconfig::ConfigException&)
        {
            return path;
        }
    }

    unsigned long Config::getULong(std::string_view setting, unsigned long def)
    {
        try
        {
            return static_cast<unsigned int>(_config.lookup(std::string{ setting }));
        }
        catch (libconfig::ConfigException&)
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
        catch (libconfig::ConfigException&)
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
        catch (libconfig::ConfigException&)
        {
            return def;
        }
    }
} // namespace lms::core