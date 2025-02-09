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

    class http_session : public boost::enable_shared_from_this<http_session> {

        /**
         * Stream
         */
        boost::beast::tcp_stream stream_;

        /**
         * Buffer
         */
        boost::beast::flat_buffer buffer_;

        /**
         * Root path
         */
        boost::shared_ptr<
                std::string const
        > doc_root_;

        /**
         * Queue limit
         */
        static constexpr std::size_t queue_limit = 64;

        /**
         * Response queue
         */
        std::queue<
                boost::beast::http::message_generator
        > response_queue_;

        /**
         * Parser
         */
        boost::optional<
                boost::beast::http::request_parser<
                        boost::beast::http::string_body
                >
        > parser_;

    public:

        /**
         * Constructor
         *
         * @param socket
         * @param doc_root
         */
        http_session(
                boost::asio::ip::tcp::socket &&socket,
                boost::shared_ptr<
                        std::string const
                > const &doc_root
        );

        /**
         * Run
         */
        void run();

    private:

        /**
         * Invoke read
         */
        void do_read();

        /**
         * Read callback
         *
         * @param ec
         * @param bytes_transferred
         */
        void on_read(
                boost::beast::error_code ec,
                std::size_t bytes_transferred
        );

        /**
         * Write
         *
         * @param response
         */
        void queue_write(
                boost::beast::http::message_generator response
        );

        /**
         * Invoke write
         */
        void do_write();

        /**
         * Write callback
         *
         * @param keep_alive
         * @param ec
         * @param bytes_transferred
         */
        void on_write(
                bool keep_alive,
                boost::beast::error_code ec,
                std::size_t bytes_transferred
        );

        /**
         * Close
         */
        void do_close();
    };

}