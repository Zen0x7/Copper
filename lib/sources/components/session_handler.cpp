#include <copper/components/session_handler.hpp>

namespace copper::components {
containers::async_of<void> session_handler(shared<state> state, uuid server_id,
                                           uuid session_id,
                                           boost::beast::tcp_stream &stream,
                                           boost::beast::flat_buffer &buffer,
                                           boost::beast::string_view doc_root) {
  auto _cs = co_await boost::asio::this_coro::cancellation_state;
  auto _executor = co_await boost::asio::this_coro::executor;

  auto _generator = boost::uuids::random_generator();

  while (!_cs.cancelled()) {
    boost::beast::http::request_parser<boost::beast::http::string_body> _parser;
    _parser.body_limit(state->get_configuration()->get()->http_body_limit_);

    auto [_ec, _] = co_await boost::beast::http::async_read(
        stream, buffer, _parser, boost::asio::as_tuple);

    auto _start_at = chronos::now();

    if (_ec == boost::beast::http::error::end_of_stream) co_return;

    if (boost::beast::websocket::is_upgrade(_parser.get())) {
      co_spawn(_executor, state->get_database()->session_is_upgrade(session_id),
               boost::asio::detached);

      get_lowest_layer(stream).expires_never();

      co_await websocket_handler(state, server_id, session_id, stream, buffer,
                                 _parser.release(), doc_root);

      co_return;
    }

    const auto _kernel = boost::make_shared<kernel>(state);

    if (_ec == boost::beast::error::timeout) {
      throw boost::system::system_error{_ec};
    }

    auto _request_id = _generator();

    std::string _ip = get_lowest_layer(stream)
                          .socket()
                          .remote_endpoint()
                          .address()
                          .to_string();

    auto [_request, _response, _generic_response] = co_await _kernel->call(
        session_id, doc_root, _parser.release(), _ip, _request_id, _start_at);

    _request->finished_at_ = chronos::now();
    _request->duration_ = _request->finished_at_ - _start_at;

    if (!_generic_response.keep_alive()) {
      co_await boost::beast::async_write(stream, std::move(_generic_response));

      co_spawn(_executor,
               state->get_database()->create_invocation(_request, _response),
               boost::asio::detached);

      state->get_logger()->requests_->info(
          "[{}] [{}] [{}] {} {} {} {}", to_string(server_id),
          to_string(session_id), to_string(_request_id), _request->version_,
          _request->method_, _request->path_, _response->status_code_);

      co_return;
    }

    co_await boost::beast::async_write(stream, std::move(_generic_response));
    co_spawn(_executor,
             state->get_database()->create_invocation(_request, _response),
             boost::asio::detached);

    state->get_logger()->requests_->info(
        "[{}] [{}] [{}] {} {} {} {}", to_string(server_id),
        to_string(session_id), to_string(_request_id), _request->version_,
        _request->method_, _request->path_, _response->status_code_);
  }
}
}  // namespace copper::components
