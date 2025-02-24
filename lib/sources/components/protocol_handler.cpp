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
  auto executor = co_await boost::asio::this_coro::executor;

  boost::beast::flat_buffer buffer;

  co_await boost::asio::this_coro::reset_cancellation_state(
      boost::asio::enable_total_cancellation(),
      boost::asio::enable_terminal_cancellation());

  co_await boost::asio::this_coro::throw_if_cancelled(false);

  stream.expires_after(std::chrono::seconds(30));

  if (co_await boost::beast::async_detect_ssl(stream, buffer)) {
    boost::asio::co_spawn(
        executor, state->get_database()->session_is_encrypted(session_id),
        boost::asio::detached);

    boost::asio::ssl::stream<typename boost::beast::tcp_stream::rebind_executor<
        boost::asio::strand<boost::asio::io_context::executor_type>>::other>
        ssl_stream{std::move(stream), ctx};

    auto bytes_transferred = co_await ssl_stream.async_handshake(
        boost::asio::ssl::stream_base::server, buffer.data());

    buffer.consume(bytes_transferred);

    co_await session_handler(state, server_id, session_id, ssl_stream, buffer,
                             doc_root);

    boost::asio::co_spawn(executor,
                          state->get_database()->session_closed(
                              session_id, "The socket was closed"),
                          boost::asio::detached);

    state->get_logger()->sessions_->info("[{}] Connection [{}] closed",
                                         to_string(server_id),
                                         to_string(session_id));

    if (!ssl_stream.lowest_layer().is_open()) co_return;

    auto [ec] = co_await ssl_stream.async_shutdown(boost::asio::as_tuple);
    if (ec && ec != boost::asio::ssl::error::stream_truncated)
      throw boost::system::system_error{ec};
  } else {
    co_await session_handler(state, server_id, session_id, stream, buffer,
                             doc_root);

    boost::asio::co_spawn(executor,
                          state->get_database()->session_closed(
                              session_id, "The socket was closed"),
                          boost::asio::detached);

    state->get_logger()->sessions_->info("[{}] Connection [{}] closed",
                                         to_string(server_id),
                                         to_string(session_id));

    if (!stream.socket().is_open()) co_return;

    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send);
  }
}

}  // namespace copper::components