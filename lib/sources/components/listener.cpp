#include <boost/asio/co_spawn.hpp>
#include <copper/components/listener.hpp>
#include <copper/components/state.hpp>

namespace copper::components {

boost::asio::awaitable<
    void, boost::asio::strand<boost::asio::io_context::executor_type>>
listener(shared<state> state, shared<task_group> task_group,
         boost::asio::ssl::context &ctx,
         boost::asio::ip::tcp::endpoint endpoint,
         boost::beast::string_view doc_root) {
  auto cs = co_await boost::asio::this_coro::cancellation_state;
  auto executor = co_await boost::asio::this_coro::executor;
  auto acceptor = typename boost::asio::ip::tcp::acceptor::rebind_executor<
      boost::asio::strand<boost::asio::io_context::executor_type>>::other{
      executor, endpoint};

  co_await boost::asio::this_coro::reset_cancellation_state(
      boost::asio::enable_total_cancellation());

  while (!cs.cancelled()) {
    auto socket_executor =
        boost::asio::make_strand(executor.get_inner_executor());
    auto [ec, socket] =
        co_await acceptor.async_accept(socket_executor, boost::asio::as_tuple);

    if (ec == boost::asio::error::operation_aborted) co_return;

    if (ec) throw boost::system::system_error{ec};

    auto session = state->get_database()->create_session(
        socket.remote_endpoint().address().to_string(),
        socket.remote_endpoint().port());

    boost::asio::co_spawn(
        std::move(socket_executor),
        detect_session(
            state, session,
            typename boost::beast::tcp_stream::rebind_executor<
                boost::asio::strand<boost::asio::io_context::executor_type>>::
                other{std::move(socket)},
            ctx, doc_root),
        task_group->adapt([session, &state](std::exception_ptr e) {
          if (e) {
            // LCOV_EXCL_START
            try {
              std::rethrow_exception(e);
            } catch (std::exception &e) {
              std::cout << "Error in session: " << e.what() << std::endl;
              state->get_database()->session_closed(session, e.what());
            }
            // LCOV_EXCL_STOP
          }
        }));
  }
}

}  // namespace copper::components