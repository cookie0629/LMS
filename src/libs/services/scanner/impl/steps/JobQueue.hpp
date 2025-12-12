
#pragma once

#include <functional>
#include <memory>
#include <span>
#include <vector>

namespace lms::core
{
    class IJob;
    class IJobScheduler;
} // namespace lms::core

namespace lms::scanner
{
    class JobQueue
    {
    public:
        using ProcessFunction = std::function<void(std::span<std::unique_ptr<core::IJob>>)>;

        // processBatchSize: 每次调用回调时最多处理多少已完成任务。
        // drainThreshold: 触发“排水”的阈值（已完成任务数 / 队列容量）。
        JobQueue(core::IJobScheduler& scheduler, std::size_t maxQueueSize, ProcessFunction processJobsDoneFunc, std::size_t processBatchSize, float drainThreshold);
        ~JobQueue();
        JobQueue(const JobQueue&) = delete;
        JobQueue& operator=(const JobQueue&) = delete;

        // push 可能会阻塞并调用回调以处理已完成任务。
        // push может блокироваться и вызывать ProcessFunction для обработки завершённых задач.
        void push(std::unique_ptr<core::IJob> job);
        void finish();

    private:
        void drainIfNeeded();

        core::IJobScheduler& _scheduler;
        const std::size_t _maxQueueSize;
        ProcessFunction _processJobsDoneFunc;
        const std::size_t _batchSize;
        const float _drainThreshold;
        std::vector<std::unique_ptr<core::IJob>> _jobsDone;
    };

} // namespace lms::scanner