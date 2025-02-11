#pragma once

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast


#include <boost/beast.hpp>
#include <queue>

#include <copper/components/report.hpp>
#include <copper/components/websocket_session.hpp>
#include <copper/components/http_kernel.hpp>

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
            Stream &stream,
            boost::beast::flat_buffer &buffer,
            boost::beast::string_view doc_root
    ) {
        auto cs = co_await boost::asio::this_coro::cancellation_state;

        while (!cs.cancelled()) {
            boost::beast::http::request_parser<boost::beast::http::string_body> parser;
            parser.body_limit(10000);

            auto [ec, _] =
                    co_await boost::beast::http::async_read(stream, buffer, parser, boost::asio::as_tuple);

            if (ec == boost::beast::http::error::end_of_stream)
                co_return;

            if (boost::beast::websocket::is_upgrade(parser.get())) {
                boost::beast::get_lowest_layer(stream).expires_never();

                co_await websocket_session_run(
                        stream, buffer, parser.release(), doc_root);

                co_return;
            }

            auto res = http_kernel::handle(doc_root, parser.release());

            if (!res.keep_alive()) {
                co_await boost::beast::async_write(stream, std::move(res));
                co_return;
            }

            co_await boost::beast::async_write(stream, std::move(res));
        }
    }

} // namespace copper::component