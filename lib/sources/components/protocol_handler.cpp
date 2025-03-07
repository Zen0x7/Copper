//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/beast/core/detect_ssl.hpp>
#include <copper/components/database.hpp>
#include <copper/components/logger.hpp>
#include <copper/components/protocol_handler.hpp>
#include <iostream>

namespace copper::components {

containers::async_of<void> protocol_handler(
    uuid server_id, uuid session_id, boost::beast::tcp_stream stream,
    boost::beast::string_view doc_root) {
  auto _executor = co_await boost::asio::this_coro::executor;

  boost::beast::flat_buffer _buffer;

  co_await boost::asio::this_coro::reset_cancellation_state(
      boost::asio::enable_total_cancellation(),
      boost::asio::enable_terminal_cancellation());

  co_await boost::asio::this_coro::throw_if_cancelled(false);

  stream.expires_after(std::chrono::seconds(30));

  if (co_await async_detect_ssl(stream, _buffer)) {
    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send);
    co_return;
  }

  co_await session_handler(server_id, session_id, stream, _buffer, doc_root);

  co_spawn(
      _executor,
      database::instance()->session_closed(session_id, "The socket was closed"),
      boost::asio::detached);

  logger::instance()->sessions_->info("[{}] Connection [{}] closed",
                                      to_string(server_id),
                                      to_string(session_id));

  if (!stream.socket().is_open()) co_return;

  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send);
}

}  // namespace copper::components