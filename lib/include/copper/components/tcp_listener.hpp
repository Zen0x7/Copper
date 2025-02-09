#pragma once

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast


#include <boost/beast.hpp>
#include <boost/asio.hpp>

#include <memory>

#include <copper/components/http_session.hpp>
#include <copper/components/report.hpp>

namespace copper::components {
    class tcp_listener : public boost::enable_shared_from_this<tcp_listener> {
        /**
         * IO Context
         */
        boost::asio::io_context &ioc_;

        /**
         * Acceptor
         */
        boost::asio::ip::tcp::acceptor acceptor_;

        /**
         * Root path
         */
        boost::shared_ptr<
                std::string const
        > doc_root_;

    public:

        /**
         * Constructor
         *
         * @param ioc
         * @param endpoint
         * @param doc_root
         */
        tcp_listener(
                boost::asio::io_context &ioc,
                boost::asio::ip::tcp::endpoint endpoint,
                boost::shared_ptr<
                        std::string const
                > const &doc_root);

        /**
         * Run
         */
        void run();

    private:

        /**
         * Invoke accept
         */
        void do_accept();

        /**
         * Accept callback
         *
         * @param ec
         * @param socket
         */
        void on_accept(
                boost::beast::error_code ec,
                boost::asio::ip::tcp::socket socket
        );
    };
}