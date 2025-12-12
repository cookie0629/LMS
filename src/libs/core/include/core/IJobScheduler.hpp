// 任务调度器接口

#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "core/LiteralString.hpp"

namespace lms::core
{
    class IJob;
    // IJobScheduler: 后台任务调度器接口，负责在线程池中调度 IJob 并跟踪完成情况。
    // IJobScheduler: интерфейс планировщика фоновых задач, выполняет IJob в пуле потоков и отслеживает их завершение.
    class IJobScheduler
    {
    public:
        virtual ~IJobScheduler() = default;

        using ShouldAbortCallback = std::function<bool()>;
        virtual void setShouldAbortCallback(ShouldAbortCallback callback) = 0;

        virtual std::size_t getThreadCount() const = 0;
        virtual void scheduleJob(std::unique_ptr<IJob> job) = 0;

        virtual std::size_t getJobsDoneCount() const = 0;
        virtual size_t popJobsDone(std::vector<std::unique_ptr<IJob>>& jobs, std::size_t maxCount) = 0;

        virtual void waitUntilJobCountAtMost(std::size_t maxOngoingJobs) = 0;
        virtual void wait() = 0;
    };

    std::unique_ptr<IJobScheduler> createJobScheduler(core::LiteralString name, std::size_t threadCount);
} // namespace lms::core