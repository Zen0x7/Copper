#pragma once

#include <boost/beast.hpp>

#include <copper/components/report.hpp>

namespace copper::components {

    class websocket_session : public boost::enable_shared_from_this<websocket_session> {

        /**
         * Stream
         */
        boost::beast::websocket::stream<
                boost::beast::tcp_stream
        > ws_;

        /**
         * Buffer
         */
        boost::beast::flat_buffer buffer_;

    public:

        /**
         * Constructor
         *
         * @param socket
         */
        explicit websocket_session(
                boost::asio::ip::tcp::socket &&socket
        );

        /**
         * Invoke accept
         *
         * @tparam Body
         * @tparam Allocator
         * @param req
         */
        template<
                class Body,
                class Allocator
        >
        void do_accept(
                boost::beast::http::request<
                        Body,
                        boost::beast::http::basic_fields<
                                Allocator
                        >
                > req
        ) {
            ws_.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));

            ws_.set_option(boost::beast::websocket::stream_base::decorator(
                    // LCOV_EXCL_START
                    [](boost::beast::websocket::response_type &res) {
                    // LCOV_EXCL_STOP
                        res.set(boost::beast::http::field::server,"Copper");
                    }));

            ws_.async_accept(req, boost::beast::bind_front_handler(&websocket_session::on_accept, shared_from_this()));
        }

    private:

        /**
         * Accept callback
         *
         * @param ec
         */
        void on_accept(
                boost::beast::error_code ec
        );

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
         * Write callback
         * @param ec
         * @param bytes_transferred
         */
        void on_write(
                boost::beast::error_code ec,
                std::size_t bytes_transferred
        );
    };
}