#ifndef COPPER_COMPONENTS_WEBSOCKET_HANDLER_HPP
#define COPPER_COMPONENTS_WEBSOCKET_HANDLER_HPP

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
containers::async_of<void> websocket_handler(
    shared<state> & /*state*/, uuid /* server_id */, uuid /* session_id */,
    Stream &stream, boost::beast::flat_buffer &buffer, request req,
    boost::beast::string_view) {
  auto _cancellation_state = co_await boost::asio::this_coro::cancellation_state;
  auto _connection = boost::beast::websocket::stream<Stream &>{stream};

  _connection.set_option(boost::beast::websocket::stream_base::timeout::suggested(
      boost::beast::role_type::server));

  _connection.set_option(boost::beast::websocket::stream_base::decorator(
      [](boost::beast::websocket::response_type &response) {
        response.set(boost::beast::http::field::server, "Copper");
      }));

  co_await _connection.async_accept(req);

  while (!_cancellation_state.cancelled()) {
    auto [_ec, _] = co_await _connection.async_read(buffer, boost::asio::as_tuple);

    if (_ec == boost::beast::websocket::error::closed ||
        _ec == boost::asio::ssl::error::stream_truncated)
      co_return;

    if (_ec) throw boost::system::system_error{_ec};

    _connection.text(_connection.got_text());
    co_await _connection.async_write(buffer.data());

    buffer.consume(buffer.size());
  }

  auto [_ec] = co_await _connection.async_close(
      boost::beast::websocket::close_code::service_restart,
      boost::asio::as_tuple);

  if (_ec && _ec != boost::asio::ssl::error::stream_truncated)
    throw boost::system::system_error{_ec};
}

}  // namespace copper::components

#endif