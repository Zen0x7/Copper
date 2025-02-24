#ifndef COPPER_COMPONENTS_HTTP_SESSION_HPP
#define COPPER_COMPONENTS_HTTP_SESSION_HPP

#pragma once

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/uuid/random_generator.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/database.hpp>
#include <copper/components/http_header.hpp>
#include <copper/components/http_kernel.hpp>
#include <copper/components/logger.hpp>
#include <copper/components/report.hpp>
#include <copper/components/state.hpp>
#include <copper/components/websocket_session.hpp>
#include <copper/models/request.hpp>
#include <copper/models/response.hpp>
#include <copper/models/session.hpp>

namespace copper::components {

/**
 * Run http session
 *
 * @tparam Stream
 * @param stream
 * @param buffer
 * @param doc_root
 * @return
 */
template <typename Stream>
containers::async_of<void> http_session_run(
    shared<state> state, uuid server_id, uuid session_id, Stream &stream,
    boost::beast::flat_buffer &buffer, boost::beast::string_view doc_root) {
  auto cs = co_await boost::asio::this_coro::cancellation_state;
  auto executor = co_await boost::asio::this_coro::executor;

  auto generator = boost::uuids::random_generator();

  while (!cs.cancelled()) {
    boost::beast::http::request_parser<boost::beast::http::string_body> parser;
    parser.body_limit(state->get_configuration()->get()->http_body_limit_);

    auto [ec, _] = co_await boost::beast::http::async_read(
        stream, buffer, parser, boost::asio::as_tuple);

    auto start_at = chronos::now();

    if (ec == boost::beast::http::error::end_of_stream) co_return;

    if (boost::beast::websocket::is_upgrade(parser.get())) {
      boost::asio::co_spawn(
          executor, state->get_database()->session_is_upgrade(session_id),
          boost::asio::detached);

      boost::beast::get_lowest_layer(stream).expires_never();

      co_await websocket_session_run(state, server_id, session_id, stream,
                                     buffer, parser.release(), doc_root);

      co_return;
    }

    auto kernel = boost::make_shared<http_kernel>(state);

    if (ec == boost::beast::error::timeout) {
      throw boost::system::system_error{ec};
    }

    auto request_id = generator();

    std::string ip = boost::beast::get_lowest_layer(stream)
                         .socket()
                         .remote_endpoint()
                         .address()
                         .to_string();

    auto [_request, _response, res] = co_await kernel->call(
        session_id, doc_root, parser.release(), ip, request_id, start_at);

    _request->finished_at_ = chronos::now();
    _request->duration_ = _request->finished_at_ - start_at;

    if (!res.keep_alive()) {
      co_await boost::beast::async_write(stream, std::move(res));

      boost::asio::co_spawn(
          executor,
          state->get_database()->create_invocation(_request, _response),
          boost::asio::detached);

      state->get_logger()->requests_->info(
          "[{}] [{}] [{}] {} {} {} {}", to_string(server_id),
          to_string(session_id), to_string(request_id), _request->version_,
          _request->method_, _request->path_, _response->status_code_);

      co_return;
    }

    co_await boost::beast::async_write(stream, std::move(res));
    boost::asio::co_spawn(
        executor, state->get_database()->create_invocation(_request, _response),
        boost::asio::detached);

    state->get_logger()->requests_->info(
        "[{}] [{}] [{}] {} {} {} {}", to_string(server_id),
        to_string(session_id), to_string(request_id), _request->version_,
        _request->method_, _request->path_, _response->status_code_);
  }
}

}  // namespace copper::components

#endif