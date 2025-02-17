#include <copper/components/detect_session.hpp>

#include <copper/components/state.hpp>

namespace copper::components {

    boost::asio::awaitable<
            void,
            boost::asio::strand<
                    boost::asio::io_context::executor_type
            >
    > detect_session(
            shared<state> & state,
            typename boost::beast::tcp_stream::rebind_executor<
                    boost::asio::strand<
                            boost::asio::io_context::executor_type
                    >
            >::other stream,
            boost::asio::ssl::context &ctx,
            boost::beast::string_view doc_root
    ) {
        boost::beast::flat_buffer buffer;

        co_await boost::asio::this_coro::reset_cancellation_state(
                boost::asio::enable_total_cancellation(), boost::asio::enable_terminal_cancellation());

        co_await boost::asio::this_coro::throw_if_cancelled(false);

        stream.expires_after(std::chrono::seconds(30));

        if (co_await boost::beast::async_detect_ssl(stream, buffer)) {
            boost::asio::ssl::stream<
                    typename boost::beast::tcp_stream::rebind_executor<boost::asio::strand<boost::asio::io_context::executor_type>>::other
            > ssl_stream{std::move(stream), ctx};

            auto bytes_transferred = co_await ssl_stream.async_handshake(
                    boost::asio::ssl::stream_base::server, buffer.data());

            buffer.consume(bytes_transferred);

            co_await http_session_run(state, ssl_stream, buffer, doc_root);

            if (!ssl_stream.lowest_layer().is_open())
                co_return;

            auto [ec] = co_await ssl_stream.async_shutdown(boost::asio::as_tuple);
            if (ec && ec != boost::asio::ssl::error::stream_truncated)
                throw boost::system::system_error{ec};
        } else {
            co_await http_session_run(state, stream, buffer, doc_root);

            if (!stream.socket().is_open())
                co_return;

            stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send);
        }
    }

} // namespace copper::component