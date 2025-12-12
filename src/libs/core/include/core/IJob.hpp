// 任务抽象接口

#pragma once

#include "core/LiteralString.hpp"

namespace lms::core
{
    // IJob: 可被调度器执行的一次性任务接口，包含任务名称和执行逻辑。
    // IJob: интерфейс однократной задачи, исполняемой планировщиком (имя + метод run).
    class IJob
    {
    public:
        virtual ~IJob() = default;

        virtual LiteralString getName() const = 0;
        virtual void run() = 0;
    };
} // namespace lms::core