
#pragma once

#include <memory>

#include <boost/asio/io_context.hpp>

#include "core/IChildProcessManager.hpp"

namespace lms::core
{
    class ChildProcessManager : public IChildProcessManager
    {
    public:
        ChildProcessManager(boost::asio::io_context& ioContext);
        ~ChildProcessManager() override = default;

        ChildProcessManager(const ChildProcessManager&) = delete;
        ChildProcessManager(ChildProcessManager&&) = delete;
        ChildProcessManager& operator=(const ChildProcessManager&) = delete;
        ChildProcessManager& operator=(ChildProcessManager&&) = delete;

    private:
        std::unique_ptr<IChildProcess> spawnChildProcess(const std::filesystem::path& path, const IChildProcess::Args& args) override;

        boost::asio::io_context& _ioContext;
    };
} // namespace lms::core