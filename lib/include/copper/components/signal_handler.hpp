#pragma once

#include <copper/components/detect_session.hpp>
#include <copper/components/task_group.hpp>

namespace copper::components {

/**
 * Signal handler
 *
 * @param task_group
 * @return
 */
boost::asio::awaitable<
    void, boost::asio::strand<boost::asio::io_context::executor_type> >
signal_handler(shared<task_group> task_group);

}  // namespace copper::components