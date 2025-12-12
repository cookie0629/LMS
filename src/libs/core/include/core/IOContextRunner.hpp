
#pragma once

#include <thread>

#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>

namespace lms::core
{
    // IOContextRunner: запускает boost::asio::io_context в пуле потоков и управляет их жизненным циклом.
    // IOContextRunner：在一组线程中运行 boost::asio::io_context，并负责其生命周期管理。
    class IOContextRunner
    {
    public:
        IOContextRunner(boost::asio::io_context& ioContext, std::size_t threadCount, std::string_view name);
        ~IOContextRunner();
        IOContextRunner(const IOContextRunner&) = delete;
        IOContextRunner& operator=(const IOContextRunner&) = delete;

        void stop();
        std::size_t getThreadCount() const;

    private:
        boost::asio::io_context& _ioContext;
        boost::asio::executor_work_guard<boost::asio::io_context::executor_type> _work;
        std::vector<std::thread> _threads;
    };
} // namespace lms::core