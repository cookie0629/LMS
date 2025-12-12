// ID 范围与分页工具定义

#pragma once

namespace lms::db
{
    template<typename IdType>
    struct IdRange
    {
    public:
        IdType first;
        IdType last;

        bool isValid() const { return first.isValid() && last.isValid(); }
    };
} // namespace lms::db
