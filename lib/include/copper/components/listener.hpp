#pragma once

#include <copper/components/detect_session.hpp>
#include <copper/components/task_group.hpp>
#include <copper/components/state.hpp>

namespace copper::components {
    boost::asio::awaitable<void, boost::asio::strand<boost::asio::io_context::executor_type>>
    listener(
            shared<state> state,
            shared<task_group> task_group,
            boost::asio::ssl::context &ctx,
            boost::asio::ip::tcp::endpoint endpoint,
            boost::beast::string_view doc_root
    );

} // namespace copper::component