#ifndef COPPER_COMPONENTS_WEBSOCKET_SESSION_HPP
#define COPPER_COMPONENTS_WEBSOCKET_SESSION_HPP

#pragma once

#include <boost/asio/as_tuple.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/detect_ssl.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/scope/scope_exit.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/report.hpp>
#include <copper/components/state.hpp>

namespace copper::components {

/**
 * Run websocket session
 *
 * @tparam Stream
 * @param stream
 * @param buffer
 * @param req
 * @return
 */
template <typename Stream>
containers::async_of<void> websocket_session_run(
  shared<state> & /* state */, uuid /* server_id */, uuid /* session_id */,
  Stream &stream, boost::beast::flat_buffer &buffer, request req,
  boost::beast::string_view) {
  auto cs = co_await boost::asio::this_coro::cancellation_state;
  auto ws = boost::beast::websocket::stream<Stream &>{stream};

  ws.set_option(boost::beast::websocket::stream_base::timeout::suggested(
      boost::beast::role_type::server));

  ws.set_option(boost::beast::websocket::stream_base::decorator(
      [](boost::beast::websocket::response_type &res) {
        res.set(boost::beast::http::field::server, "Copper");
      }));

  co_await ws.async_accept(req);

  while (!cs.cancelled()) {
    auto [ec, _] = co_await ws.async_read(buffer, boost::asio::as_tuple);

    if (ec == boost::beast::websocket::error::closed ||
        ec == boost::asio::ssl::error::stream_truncated)
      co_return;

    if (ec) throw boost::system::system_error{ec};

    ws.text(ws.got_text());
    co_await ws.async_write(buffer.data());

    buffer.consume(buffer.size());
  }

  auto [ec] = co_await ws.async_close(
      boost::beast::websocket::close_code::service_restart,
      boost::asio::as_tuple);

  if (ec && ec != boost::asio::ssl::error::stream_truncated)
    throw boost::system::system_error{ec};
}

}  // namespace copper::components

#endif