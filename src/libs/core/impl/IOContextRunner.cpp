
#include "core/IOContextRunner.hpp"

#include <cstdlib>

#include "core/ILogger.hpp"
#include "core/ITraceLogger.hpp"

namespace lms::core
{
    // IOContextRunner: 构造时立即启动多个线程，每个线程都调用 io_context.run()。
    // IOContextRunner: при создании сразу запускает несколько потоков, каждый крутит io_context.run().
    IOContextRunner::IOContextRunner(boost::asio::io_context& ioContext, std::size_t threadCount, std::string_view name)
        : _ioContext{ ioContext }
        , _work{ boost::asio::make_work_guard(ioContext) }
    {
        LMS_LOG(UTILS, INFO, "Starting IO context with " << threadCount << " threads...");

        for (std::size_t i{}; i < threadCount; ++i)
        {
            std::string threadName{ name };
            if (!threadName.empty())
            {
                threadName += "Thread_";
                threadName += std::to_string(i);
            }

            _threads.emplace_back([this, threadName] {
                if (!threadName.empty())
                {
                    if (auto* traceLogger{ Service<tracing::ITraceLogger>::get() })
                        traceLogger->setThreadName(std::this_thread::get_id(), threadName);
                }

                try
                {
                    _ioContext.run();
                }
                catch (const std::exception& e)
                {
                    LMS_LOG(UTILS, FATAL, "Exception caught in IO context: " << e.what());
                    std::abort();
                }
            });
        }
    }

    void IOContextRunner::stop()
    {
        LMS_LOG(UTILS, DEBUG, "Stopping IO context...");
        _work.reset();
        _ioContext.stop();
        LMS_LOG(UTILS, DEBUG, "IO context stopped!");
    }

    std::size_t IOContextRunner::getThreadCount() const
    {
        return _threads.size();
    }

    IOContextRunner::~IOContextRunner()
    {
        stop();

        for (std::thread& t : _threads)
            t.join();
    }
} // namespace lms::core