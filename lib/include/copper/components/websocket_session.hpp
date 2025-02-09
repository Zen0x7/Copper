#pragma once

#include <boost/beast.hpp>

#include <copper/components/report.hpp>

namespace copper::components {

    class websocket_session : public boost::enable_shared_from_this<websocket_session>
    {
        boost::beast::websocket::stream<boost::beast::tcp_stream> ws_;
        boost::beast::flat_buffer buffer_;

    public:
        explicit websocket_session(boost::asio::ip::tcp::socket&& socket) : ws_(std::move(socket))
        { }

        template<class Body, class Allocator>
        void do_accept(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
        {
            ws_.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));

            ws_.set_option(boost::beast::websocket::stream_base::decorator(
                    [](boost::beast::websocket::response_type& res)
                    {
                        res.set(boost::beast::http::field::server,
                                std::string(BOOST_BEAST_VERSION_STRING) +
                                " advanced-server");
                    }));

            ws_.async_accept(req, boost::beast::bind_front_handler(&websocket_session::on_accept, shared_from_this()));
        }

    private:
        void on_accept(boost::beast::error_code ec)
        {
            if(ec)
                return report(ec, "accept");

            do_read();
        }

        void do_read()
        {
            ws_.async_read(buffer_, boost::beast::bind_front_handler(&websocket_session::on_read, shared_from_this()));
        }

        void on_read(boost::beast::error_code ec, std::size_t bytes_transferred)
        {
            boost::ignore_unused(bytes_transferred);

            if(ec == boost::beast::websocket::error::closed)
                return;

            if(ec)
                report(ec, "read");

            ws_.text(ws_.got_text());
            ws_.async_write(buffer_.data(),boost::beast::bind_front_handler(&websocket_session::on_write, shared_from_this()));
        }

        void on_write(boost::beast::error_code ec, std::size_t bytes_transferred)
        {
            boost::ignore_unused(bytes_transferred);

            if(ec)
                return report(ec, "write");

            buffer_.consume(buffer_.size());

            do_read();
        }
    };
}