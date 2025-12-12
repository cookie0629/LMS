// 配置读取接口定义
#pragma once

#include <filesystem>
#include <functional>
#include <string_view>

namespace lms::core
{
    // IConfig: интерфейс для доступа к настройкам из конфигурационного файла (libconfig++).
    // IConfig：用于从配置文件（libconfig++）中读取配置项的接口。
    class IConfig
    {
    public:
        virtual ~IConfig() = default;

        // getString: возвращает строковое значение конфигурации, либо значение по умолчанию, если ключ отсутствует.
        // getString：读取字符串配置，如果键不存在则返回默认值。
        virtual std::string_view getString(std::string_view setting, std::string_view def) = 0;

        // visitStrings: обходит список строк (массив/列表) в конфиге и для каждого элемента вызывает _func。
        // visitStrings：遍历配置中的字符串列表，对每个元素调用回调 _func；如果键不存在，则遍历默认列表 defs。
        virtual void visitStrings(std::string_view setting, std::function<void(std::string_view)> _func, std::initializer_list<std::string_view> def) = 0;

        // getPath: читает путь (filesystem::path) из конфигурации, либо возвращает путь по умолчанию。
        // getPath：从配置中读取路径（filesystem::path），若没有配置则返回默认路径。
        virtual std::filesystem::path getPath(std::string_view setting, const std::filesystem::path& def) = 0;

        // getULong/getLong: читают целочисленные значения (unsigned long / long) с значением по умолчанию.
        // getULong/getLong：读取整型配置（无符号/有符号），若键不存在则使用默认值。
        virtual unsigned long getULong(std::string_view setting, unsigned long def) = 0;
        virtual long getLong(std::string_view setting, long def) = 0;

        // getBool: читает булево значение из конфигурации, либо возвращает def, если ключ не найден.
        // getBool：读取布尔型配置，如果键缺失则返回默认值 def。
        virtual bool getBool(std::string_view setting, bool def) = 0;
    };

    // createConfig: создаёт конкретную реализацию IConfig (см. impl/Config.{hpp,cpp}).
    // createConfig：创建 IConfig 的具体实现（见 impl/Config.{hpp,cpp}）。
    std::unique_ptr<IConfig> createConfig(const std::filesystem::path& p);
} // namespace lms::core