#pragma once

#include <copper/components/task_group.hpp>
#include <copper/components/detect_session.hpp>

namespace copper::components {
    boost::asio::awaitable<
            void, boost::asio::strand<boost::asio::io_context::executor_type>
    > signal_handler(task_group &task_group);
}