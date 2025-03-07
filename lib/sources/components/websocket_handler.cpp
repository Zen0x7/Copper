//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/asio/as_tuple.hpp>
#include <boost/beast/core/detect_ssl.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <copper/components/state.hpp>
#include <copper/components/websocket.hpp>
#include <copper/components/websocket_handler.hpp>
#include <iostream>

namespace copper::components {
containers::async_of<void> websocket_handler(uuid session_id, uuid,
                                             boost::beast::tcp_stream &stream,
                                             boost::beast::flat_buffer &buffer,
                                             request req,
                                             boost::beast::string_view) {
  auto _cancellation_state =
      co_await boost::asio::this_coro::cancellation_state;

  auto _websocket =
      boost::make_shared<websocket>(session_id, stream.release_socket());

  state::instance()->connected(_websocket);

  _websocket->stream_.set_option(
      boost::beast::websocket::stream_base::timeout::suggested(
          boost::beast::role_type::server));

  _websocket->stream_.set_option(
      boost::beast::websocket::stream_base::decorator(
          [](boost::beast::websocket::response_type &response) {
            response.set(boost::beast::http::field::server, "Copper");
          }));

  co_await _websocket->stream_.async_accept(req);

  while (!_cancellation_state.cancelled()) {
    auto [_ec, _] =
        co_await _websocket->stream_.async_read(buffer, boost::asio::as_tuple);

    if (_ec == boost::beast::websocket::error::closed ||
        _ec == boost::asio::ssl::error::stream_truncated)
      co_return;

    if (_ec) throw boost::system::system_error{_ec};

    _websocket->stream_.text(_websocket->stream_.got_text());

    co_await _websocket->stream_.async_write(buffer.data());

    buffer.consume(buffer.size());
  }

  auto [_ec] = co_await _websocket->stream_.async_close(
      boost::beast::websocket::close_code::service_restart,
      boost::asio::as_tuple);

  if (_ec && _ec != boost::asio::ssl::error::stream_truncated)
    throw boost::system::system_error{_ec};
}
}  // namespace copper::components
