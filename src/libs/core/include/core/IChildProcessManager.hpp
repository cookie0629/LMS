// 子进程管理接口
#pragma once

#include <filesystem>
#include <memory>

#include <boost/asio/io_context.hpp>

#include "IChildProcess.hpp"

namespace lms::core
{
    // IChildProcessManager: фабрика/менеджер子进程，负责在给定路径上启动外部程序。
    // IChildProcessManager：子进程管理器，用于在指定路径启动外部命令。
    class IChildProcessManager
    {
    public:
        virtual ~IChildProcessManager() = default;

        virtual std::unique_ptr<IChildProcess> spawnChildProcess(const std::filesystem::path& path, const IChildProcess::Args& args) = 0;
    };

    std::unique_ptr<IChildProcessManager> createChildProcessManager(boost::asio::io_context& ioContext);
} // namespace lms::core