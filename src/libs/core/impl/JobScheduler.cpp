
#include "JobScheduler.hpp"

#include <boost/asio/post.hpp>

#include "core/ITraceLogger.hpp"

#include "core/IJob.hpp"

namespace lms::core
{
    std::unique_ptr<IJobScheduler> createJobScheduler(core::LiteralString name, std::size_t threadCount)
    {
        return std::make_unique<JobScheduler>(name, threadCount);
    }

    JobScheduler::JobScheduler(core::LiteralString name, std::size_t threadCount)
        : _name{ name }
        , _ioContextRunner{ _ioContext, threadCount, name.str() }
    {
    }

    JobScheduler::~JobScheduler() = default;

    void JobScheduler::setShouldAbortCallback(ShouldAbortCallback callback)
    {
        _abortCallback = callback;
    }

    std::size_t JobScheduler::getThreadCount() const
    {
        return _ioContextRunner.getThreadCount();
    }

    void JobScheduler::scheduleJob(std::unique_ptr<IJob> job)
    {
        {
            std::scoped_lock lock{ _mutex };
            _ongoingJobCount += 1;
        }

        auto jobHandler{ [job = std::move(job), this]() mutable {
            if (_abortCallback && _abortCallback())
            {
                std::scoped_lock lock{ _mutex };
                _ongoingJobCount -= 1;
            }
            else
            {
                {
                    LMS_SCOPED_TRACE_OVERVIEW(_name, job->getName());
                    job->run();
                }

                {
                    std::scoped_lock lock{ _mutex };

                    _doneJobs.emplace_back(std::move(job));
                    _ongoingJobCount -= 1;
                }
            }

            _condVar.notify_all();
        } };

        boost::asio::post(_ioContext, std::move(jobHandler));
    }

    std::size_t JobScheduler::getJobsDoneCount() const
    {
        std::scoped_lock lock{ _mutex };
        return _doneJobs.size();
    }

    size_t JobScheduler::popJobsDone(std::vector<std::unique_ptr<IJob>>& doneJobs, std::size_t maxCount)
    {
        doneJobs.clear();
        doneJobs.reserve(maxCount);

        {
            std::scoped_lock lock{ _mutex };

            while (doneJobs.size() < maxCount && !_doneJobs.empty())
            {
                doneJobs.push_back(std::move(_doneJobs.front()));
                _doneJobs.pop_front();
            }
        }

        return doneJobs.size();
    }

    void JobScheduler::waitUntilJobCountAtMost(std::size_t maxOngoingJobs)
    {
        if (_ongoingJobCount <= maxOngoingJobs)
            return;

        {
            LMS_SCOPED_TRACE_OVERVIEW(_name, "WaitJobs");

            std::unique_lock lock{ _mutex };
            _condVar.wait(lock, [=, this] { return _ongoingJobCount <= maxOngoingJobs; });
        }
    }

    void JobScheduler::wait()
    {
        waitUntilJobCountAtMost(0);
    }

} // namespace lms::core
