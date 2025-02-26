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
  auto _cs = co_await boost::asio::this_coro::cancellation_state;
  auto _executor = co_await boost::asio::this_coro::executor;
  auto _acceptor = typename boost::asio::ip::tcp::acceptor::rebind_executor<
      boost::asio::strand<boost::asio::io_context::executor_type>>::other{
      _executor, endpoint};

  co_await boost::asio::this_coro::reset_cancellation_state(
      boost::asio::enable_total_cancellation());

  auto _generator = boost::uuids::random_generator();
  auto _transaction_id = _generator();

  json::object _server_registered = {
      {"transaction_id", to_string(_transaction_id)},
      {"event", "server_registered"},
      {"data", {{"id", to_string(server_id)}}}};

  co_await state->get_cache()->publish("events", serialize(_server_registered));

  state->get_logger()->system_->info("[{}] Server is open",
                                     to_string(server_id));

  while (!_cs.cancelled()) {
    auto _socket_executor = make_strand(_executor.get_inner_executor());
    auto [_ec, _socket] = co_await _acceptor.async_accept(
        _socket_executor, boost::asio::as_tuple);

    if (_ec == boost::asio::error::operation_aborted) co_return;

    if (_ec) throw boost::system::system_error{_ec};

    auto _session_id = _generator();

    state->get_logger()->sessions_->info(
        "[{}] Connection [{}] from [{}:{}] accepted", to_string(server_id),
        to_string(_session_id), _socket.remote_endpoint().address().to_string(),
        _socket.remote_endpoint().port());

    co_spawn(_executor,
             state->get_database()->create_session(
                 _session_id, _socket.remote_endpoint().address().to_string(),
                 _socket.remote_endpoint().port()),
             boost::asio::detached);

    co_spawn(
        std::move(_socket_executor),
        protocol_handler(
            state, server_id, _session_id,
            typename boost::beast::tcp_stream::rebind_executor<
                boost::asio::strand<boost::asio::io_context::executor_type>>::
                other{std::move(_socket)},
            ctx, doc_root),

        // LCOV_EXCL_START
        task_group->adapt([server_id, _session_id, _executor,
                           &state](std::exception_ptr e) {
          // LCOV_EXCL_STOP

          if (e) {
            // LCOV_EXCL_START
            try {
              std::rethrow_exception(e);
            } catch (std::exception &e) {
              co_spawn(
                  _executor,
                  state->get_database()->session_closed(_session_id, e.what()),
                  boost::asio::detached);

              state->get_logger()->sessions_->info(
                  "[{}] Connection [{}] error [{}]", to_string(server_id),
                  to_string(_session_id), e.what());
            }
            // LCOV_EXCL_STOP
          }
        }));
  }
}

}  // namespace copper::components