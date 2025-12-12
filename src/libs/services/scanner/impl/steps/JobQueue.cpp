
#include "JobQueue.hpp"

#include <cassert>

#include "core/IJob.hpp"
#include "core/IJobScheduler.hpp"

namespace lms::scanner
{
    JobQueue::JobQueue(core::IJobScheduler& scheduler, std::size_t maxQueueSize, ProcessFunction processJobsDoneFunc, std::size_t batchSize, float _drainThreshold)
        : _scheduler{ scheduler }
        , _maxQueueSize{ maxQueueSize }
        , _processJobsDoneFunc{ std::move(processJobsDoneFunc) }
        , _batchSize{ batchSize }
        , _drainThreshold{ _drainThreshold }
    {
        assert(_scheduler.getJobsDoneCount() == 0);
    }

    JobQueue::~JobQueue()
    {
        finish();
    }

    void JobQueue::push(std::unique_ptr<core::IJob> job)
    {
        _scheduler.scheduleJob(std::move(job));
        drainIfNeeded();
    }

    void JobQueue::finish()
    {
        _scheduler.wait();
        while (_scheduler.popJobsDone(_jobsDone, _batchSize) > 0)
        {
            _processJobsDoneFunc(std::span{ _jobsDone });
            _jobsDone.clear();
        }
    }

    void JobQueue::drainIfNeeded()
    {
        while (_scheduler.getJobsDoneCount() > static_cast<std::size_t>(_maxQueueSize * _drainThreshold))
        {
            _scheduler.popJobsDone(_jobsDone, _batchSize);
            _processJobsDoneFunc(std::span{ _jobsDone });
            _jobsDone.clear();
        }
        _scheduler.waitUntilJobCountAtMost(_maxQueueSize);
    }
} // namespace lms::scanner