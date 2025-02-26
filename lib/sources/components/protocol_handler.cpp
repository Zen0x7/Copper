#include <copper/components/logger.hpp>
#include <copper/components/protocol_handler.hpp>
#include <copper/components/state.hpp>

namespace copper::components {

containers::async_of<void> protocol_handler(
    shared<state> state, uuid server_id, uuid session_id,
    typename boost::beast::tcp_stream::rebind_executor<
        boost::asio::strand<boost::asio::io_context::executor_type>>::other
        stream,
    boost::asio::ssl::context &ctx, boost::beast::string_view doc_root) {
  auto _executor = co_await boost::asio::this_coro::executor;

  boost::beast::flat_buffer _buffer;

  co_await boost::asio::this_coro::reset_cancellation_state(
      boost::asio::enable_total_cancellation(),
      boost::asio::enable_terminal_cancellation());

  co_await boost::asio::this_coro::throw_if_cancelled(false);

  stream.expires_after(std::chrono::seconds(30));

  if (co_await async_detect_ssl(stream, _buffer)) {
    co_spawn(_executor, state->get_database()->session_is_encrypted(session_id),
             boost::asio::detached);

    boost::asio::ssl::stream<typename boost::beast::tcp_stream::rebind_executor<
        boost::asio::strand<boost::asio::io_context::executor_type>>::other>
        _ssl_stream{std::move(stream), ctx};

    auto _bytes_transferred = co_await _ssl_stream.async_handshake(
        boost::asio::ssl::stream_base::server, _buffer.data());

    _buffer.consume(_bytes_transferred);

    co_await session_handler(state, server_id, session_id, _ssl_stream, _buffer,
                             doc_root);

    co_spawn(_executor,
             state->get_database()->session_closed(session_id,
                                                   "The socket was closed"),
             boost::asio::detached);

    state->get_logger()->sessions_->info("[{}] Connection [{}] closed",
                                         to_string(server_id),
                                         to_string(session_id));

    if (!_ssl_stream.lowest_layer().is_open()) co_return;

    if (auto [_ec] = co_await _ssl_stream.async_shutdown(boost::asio::as_tuple); _ec && _ec != boost::asio::ssl::error::stream_truncated)
      throw boost::system::system_error{_ec};
  } else {
    co_await session_handler(state, server_id, session_id, stream, _buffer,
                             doc_root);

    co_spawn(_executor,
             state->get_database()->session_closed(session_id,
                                                   "The socket was closed"),
             boost::asio::detached);

    state->get_logger()->sessions_->info("[{}] Connection [{}] closed",
                                         to_string(server_id),
                                         to_string(session_id));

    if (!stream.socket().is_open()) co_return;

    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send);
  }
}

}  // namespace copper::components