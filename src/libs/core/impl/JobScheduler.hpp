// 任务调度与线程池执行声明

#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>

#include "core/IJobScheduler.hpp"
#include "core/IOContextRunner.hpp"

namespace lms::core
{
    // JobScheduler: 基于 boost::asio 的任务调度器实现，在独立线程中执行任务并支持等待/中止。
    // JobScheduler: реализация IJobScheduler на базе boost::asio, выполняет задачи в отдельных потоках и позволяет ждать/отменять.
    class JobScheduler : public IJobScheduler
    {
    public:
        JobScheduler(core::LiteralString name, std::size_t threadCount);
        ~JobScheduler() override;
        JobScheduler(const JobScheduler&) = delete;
        JobScheduler& operator=(const JobScheduler&) = delete;

    private:
        void setShouldAbortCallback(ShouldAbortCallback callback) override;

        std::size_t getThreadCount() const override;
        void scheduleJob(std::unique_ptr<IJob> job) override;

        std::size_t getJobsDoneCount() const override;
        size_t popJobsDone(std::vector<std::unique_ptr<IJob>>& jobs, std::size_t maxCount) override;

        void waitUntilJobCountAtMost(std::size_t maxOngoingJobs) override;
        void wait() override;

        core::LiteralString _name;
        boost::asio::io_context _ioContext;
        core::IOContextRunner _ioContextRunner;

        ShouldAbortCallback _abortCallback;

        mutable std::mutex _mutex;
        std::atomic<std::size_t> _ongoingJobCount;
        std::deque<std::unique_ptr<IJob>> _doneJobs;
        std::condition_variable _condVar;
    };
} // namespace lms::core