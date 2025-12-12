// 强类型 ID 定义与运算

#pragma once

#include <string>

namespace lms::db
{
    class IdType
    {
    public:
        using ValueType = long long;

        IdType();
        IdType(ValueType id);

        bool isValid() const;
        std::string toString() const;

        ValueType getValue() const { return _id; }
        auto operator<=>(const IdType& other) const = default;

    private:
        ValueType _id;
    };

#define LMS_DECLARE_IDTYPE(name)                                             \
    namespace lms::db                                                        \
    {                                                                        \
        class name : public IdType                                           \
        {                                                                    \
        public:                                                              \
            using IdType::IdType;                                            \
            auto operator<=>(const name& other) const = default;             \
        };                                                                   \
    }                                                                        \
    namespace std                                                            \
    {                                                                        \
        template<>                                                           \
        class hash<lms::db::name>                                            \
        {                                                                    \
        public:                                                              \
            size_t operator()(lms::db::name id) const                        \
            {                                                                \
                return std::hash<lms::db::name::ValueType>()(id.getValue()); \
            }                                                                \
        };                                                                   \
    } // ns std
} // namespace lms::db
