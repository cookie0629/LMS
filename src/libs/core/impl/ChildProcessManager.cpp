// 子进程生命周期与池化管理实现

#include "ChildProcessManager.hpp"

#include "core/ILogger.hpp"

#include "ChildProcess.hpp"

namespace lms::core
{
    std::unique_ptr<IChildProcessManager> createChildProcessManager(boost::asio::io_context& ioContext)
    {
        return std::make_unique<ChildProcessManager>(ioContext);
    }

    ChildProcessManager::ChildProcessManager(boost::asio::io_context& ioContext)
        : _ioContext{ ioContext }
    {
    }

    std::unique_ptr<IChildProcess> ChildProcessManager::spawnChildProcess(const std::filesystem::path& path, const IChildProcess::Args& args)
    {
        return std::make_unique<ChildProcess>(_ioContext, path, args);
    }
} // namespace lms::core