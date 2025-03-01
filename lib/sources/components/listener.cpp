#include <boost/asio/co_spawn.hpp>
#include <copper/components/cache.hpp>
#include <copper/components/listener.hpp>
#include <copper/components/logger.hpp>
#include <copper/components/state.hpp>
#include <iostream>

namespace copper::components {

containers::async_of<void> listener(boost::uuids::uuid server_id,
                                    shared<state> state,
                                    shared<task_group> task_group,
                                    boost::asio::ip::tcp::endpoint endpoint,
                                    boost::beast::string_view doc_root) {
  auto _cs = co_await boost::asio::this_coro::cancellation_state;
  auto _executor = co_await boost::asio::this_coro::executor;
  auto _acceptor = boost::asio::ip::tcp::acceptor{_executor, endpoint};

  co_await boost::asio::this_coro::reset_cancellation_state(
      boost::asio::enable_total_cancellation());

  auto _generator = boost::uuids::random_generator();
  auto _transaction_id = _generator();

  json::object _server_registered = {
      {"transaction_id", to_string(_transaction_id)},
      {"event", "server_registered"},
      {"data", {{"id", to_string(server_id)}}}};

  co_await cache::instance()->publish("events", serialize(_server_registered));

  logger::instance()->system_->info("[{}] Server is open",
                                    to_string(server_id));

  while (!_cs.cancelled()) {
    auto _socket_executor = make_strand(_executor.get_inner_executor());
    auto [_ec, _socket] = co_await _acceptor.async_accept(
        _socket_executor, boost::asio::as_tuple);

    if (_ec == boost::asio::error::operation_aborted) co_return;

    if (_ec) throw boost::system::system_error{_ec};

    auto _session_id = _generator();

    logger::instance()->sessions_->info(
        "[{}] Connection [{}] from [{}:{}] accepted", to_string(server_id),
        to_string(_session_id), _socket.remote_endpoint().address().to_string(),
        _socket.remote_endpoint().port());

    co_spawn(_executor,
             state->get_database()->create_session(
                 _session_id, _socket.remote_endpoint().address().to_string(),
                 _socket.remote_endpoint().port()),
             boost::asio::detached);

    // LCOV_EXCL_START
    co_spawn(std::move(_socket_executor),
             protocol_handler(state, server_id, _session_id,
                              boost::beast::tcp_stream{std::move(_socket)},
                              doc_root),

             task_group->adapt([server_id, _session_id, _executor,
                                &state](const std::exception_ptr &e) {
               if (e) {
                 try {
                   std::rethrow_exception(e);
                 } catch (std::exception &exception) {
                   std::cout << "Exception happens" << std::endl;
                   co_spawn(_executor,
                            state->get_database()->session_closed(
                                _session_id, exception.what()),
                            boost::asio::detached);

                   logger::instance()->sessions_->info(
                       "[{}] Connection [{}] error [{}]", to_string(server_id),
                       to_string(_session_id), exception.what());
                 }
               }
             }));
    // LCOV_EXCL_STOP
  }
}

}  // namespace copper::components