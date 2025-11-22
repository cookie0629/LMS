#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>

#include "core/TaggedType.hpp"

namespace lms::db
{
    /**
     * @brief 数据库ID类型基类
     */
    class IdType : public core::TaggedType<std::int64_t, struct IdTypeTag>
    {
    public:
        using ValueType = std::int64_t;
        using TaggedType::TaggedType;
        
        IdType() = default;
        explicit IdType(ValueType id) : TaggedType(id) {}
        
        ValueType getValue() const { return TaggedType::getValue(); }
    };

} // namespace lms::db

/**
 * @brief 声明ID类型宏
 * 
 * 使用示例：
 * LMS_DECLARE_IDTYPE(UserId)
 */
#define LMS_DECLARE_IDTYPE(TypeName) \
    namespace lms::db \
    { \
        class TypeName : public IdType \
        { \
        public: \
            using IdType::IdType; \
        }; \
    }

