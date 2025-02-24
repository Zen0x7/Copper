#include <boost/asio/co_spawn.hpp>
#include <copper/components/cache.hpp>
#include <copper/components/listener.hpp>
#include <copper/components/logger.hpp>
#include <copper/components/state.hpp>

namespace copper::components {

containers::async_of<void> listener(boost::uuids::uuid server_id,
                                    shared<state> state,
                                    shared<task_group> task_group,
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

  auto uuid_generator = boost::uuids::random_generator();
  auto transaction_id = uuid_generator();

  json::object server_registered = {
      {"transaction_id", to_string(transaction_id)},
      {"event", "server_registered"},
      {"data", {{"id", to_string(server_id)}}}};

  co_await state->get_cache()->publish("events", serialize(server_registered));

  state->get_logger()->system_->info("[{}] Server is open",
                                     to_string(server_id));

  while (!cs.cancelled()) {
    auto socket_executor =
        boost::asio::make_strand(executor.get_inner_executor());
    auto [ec, socket] =
        co_await acceptor.async_accept(socket_executor, boost::asio::as_tuple);

    if (ec == boost::asio::error::operation_aborted) co_return;

    if (ec) throw boost::system::system_error{ec};

    auto session_id = uuid_generator();

    state->get_logger()->sessions_->info(
        "[{}] Connection [{}] from [{}:{}] accepted", to_string(server_id),
        to_string(session_id), socket.remote_endpoint().address().to_string(),
        socket.remote_endpoint().port());

    boost::asio::co_spawn(
        executor,
        state->get_database()->create_session(
            session_id, socket.remote_endpoint().address().to_string(),
            socket.remote_endpoint().port()),
        boost::asio::detached);

    boost::asio::co_spawn(
        std::move(socket_executor),
        protocol_handler(
            state, server_id, session_id,
            typename boost::beast::tcp_stream::rebind_executor<
                boost::asio::strand<boost::asio::io_context::executor_type>>::
                other{std::move(socket)},
            ctx, doc_root),

        // LCOV_EXCL_START
        task_group->adapt([server_id, session_id, executor,
                           &state](std::exception_ptr e) {
          // LCOV_EXCL_STOP

          if (e) {
            // LCOV_EXCL_START
            try {
              std::rethrow_exception(e);
            } catch (std::exception &e) {
              boost::asio::co_spawn(
                  executor,
                  state->get_database()->session_closed(session_id, e.what()),
                  boost::asio::detached);

              state->get_logger()->sessions_->info(
                  "[{}] Connection [{}] error [{}]", to_string(server_id),
                  to_string(session_id), e.what());
            }
            // LCOV_EXCL_STOP
          }
        }));
  }
}

}  // namespace copper::components