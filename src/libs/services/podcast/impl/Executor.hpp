
#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>

namespace lms::podcast
{
    class Executor
    {
    public:
        Executor(boost::asio::io_context& ioContext);

        void post(std::function<void()> callback);

    private:
        boost::asio::io_context::strand _strand;
    };

} // namespace lms::podcast