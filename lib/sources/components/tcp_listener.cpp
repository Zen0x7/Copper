#include <copper/components/tcp_listener.hpp>

namespace copper::components {

    tcp_listener::tcp_listener(
            boost::asio::io_context &ioc,
            boost::asio::ip::tcp::endpoint endpoint,
            const boost::shared_ptr<
                    const std::string
            > &doc_root
    )
            : ioc_(ioc),
              acceptor_(boost::asio::make_strand(ioc)),
              doc_root_(doc_root) {
        boost::beast::error_code ec;

        acceptor_.open(endpoint.protocol(), ec);
        if (ec) {
            report(ec, "open");
            return;
        }

        acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
        if (ec) {
            report(ec, "set_option");
            return;
        }

        acceptor_.bind(endpoint, ec);
        if (ec) {
            report(ec, "bind");
            return;
        }

        acceptor_.listen(
                boost::asio::socket_base::max_listen_connections, ec);
        if (ec) {
            report(ec, "listen");
            return;
        }
    }

    void tcp_listener::run() {
        boost::asio::dispatch(
                acceptor_.get_executor(),
                boost::beast::bind_front_handler(
                        &tcp_listener::do_accept,
                        this->shared_from_this()));
    }

    void tcp_listener::do_accept() {
        acceptor_.async_accept(
                boost::asio::make_strand(ioc_),
                boost::beast::bind_front_handler(
                        &tcp_listener::on_accept,
                        shared_from_this()));
    }

    void tcp_listener::on_accept(
            boost::beast::error_code ec,
            boost::asio::ip::tcp::socket socket
    ) {
        if (ec) {
            report(ec, "accept");
        } else {
            boost::make_shared<http_session>(
                    std::move(socket),
                    doc_root_)->run();
        }

        do_accept();
    }
}
