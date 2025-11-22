#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace lms::core
{
    /**
     * @brief UUID 类，用于生成和表示唯一标识符
     * 
     * UUID 格式：xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
     * 例如：123e4567-e89b-12d3-a456-426614174000
     */
    class UUID
    {
    public:
        /**
         * @brief 从字符串创建 UUID
         * @param str UUID 字符串
         * @return UUID 对象，如果字符串格式无效则返回 std::nullopt
         */
        static std::optional<UUID> fromString(std::string_view str);

        /**
         * @brief 生成新的 UUID v4（随机 UUID）
         * @return 新生成的 UUID
         */
        static UUID generate();

        /**
         * @brief 获取 UUID 的字符串表示
         * @return UUID 字符串（小写格式）
         */
        std::string_view getAsString() const { return _value; }

        /**
         * @brief 比较操作符
         */
        auto operator<=>(const UUID&) const = default;
        bool operator==(const UUID&) const = default;

    private:
        UUID(std::string_view value);
        std::string _value;
    };
} // namespace lms::core

