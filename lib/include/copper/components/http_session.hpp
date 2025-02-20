#pragma once

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast


#include <copper/components/report.hpp>
#include <copper/components/websocket_session.hpp>

#include <copper/components/http_kernel.hpp>
#include <copper/components/http_header.hpp>

#include <copper/components/chronos.hpp>
#include <app/models/session.hpp>
#include <app/models/request.hpp>
#include <app/models/response.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/asio/co_spawn.hpp>

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
    template<
      typename Stream
    >
    boost::asio::awaitable<
      void,
      boost::asio::strand<
        boost::asio::io_context::executor_type
      >
    > http_session_run(
      shared<state> state,
      shared<app::models::session> session,
      Stream &stream,
      boost::beast::flat_buffer &buffer,
      boost::beast::string_view doc_root
    ) {
      auto cs = co_await boost::asio::this_coro::cancellation_state;

      auto generator = boost::uuids::random_generator();

      while (!cs.cancelled()) {
        boost::beast::http::request_parser<boost::beast::http::string_body> parser;
        parser.body_limit(std::stoi(dotenv::getenv("HTTP_BODY_LIMIT", "10000")));

        auto [ec, _] =
          co_await boost::beast::http::async_read(stream, buffer, parser, boost::asio::as_tuple);

        auto start_at = chronos::now();

        if (ec == boost::beast::http::error::end_of_stream)
          co_return;

        if (boost::beast::websocket::is_upgrade(parser.get())) {

          state
            ->get_database()
            ->session_is_upgrade(session);

          boost::beast::get_lowest_layer(stream).expires_never();

          co_await websocket_session_run(
            state, stream, buffer, parser.release(), doc_root);

          co_return;
        }

        auto kernel = boost::make_shared<http_kernel>(state);

        if (ec == boost::beast::error::timeout) {
          throw boost::system::system_error{ec};
        }

        auto request_id = generator();

        std::string ip = boost::beast::get_lowest_layer(stream).socket().remote_endpoint().address().to_string();

        auto [_request, _response, res] = co_await kernel->invoke(session, doc_root, parser.release(), ip, request_id, start_at);

        _request->finished_at_ = chronos::now();
        _request->duration_ = _request->finished_at_ - start_at;

        if (!res.keep_alive()) {
          co_await boost::beast::async_write(stream, std::move(res));
          co_await state->get_database()->create_request(_request, _response);
          co_return;
        }

        co_await boost::beast::async_write(stream, std::move(res));
        co_await state->get_database()->create_request(_request, _response);
      }
    }

} // namespace copper::component