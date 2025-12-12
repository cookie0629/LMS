
#include "Executor.hpp"

#include <boost/asio/io_context.hpp>

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/post.hpp>

namespace lms::podcast
{
    Executor::Executor(boost::asio::io_context& ioContext)
        : _strand{ ioContext }
    {
    }

    void Executor::post(std::function<void()> callback)
    {
        boost::asio::post(boost::asio::bind_executor(_strand, std::move(callback)));
    }
} // namespace lms::podcast