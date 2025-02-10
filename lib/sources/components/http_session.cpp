#include <copper/components/http_session.hpp>

namespace copper::components {

    http_session::http_session(
            boost::asio::ip::tcp::socket &&socket,
            const boost::shared_ptr<
                    const std::string
            > &doc_root
    ) : stream_(std::move(socket)),
        doc_root_(doc_root) {
        static_assert(queue_limit > 0,
                      "queue limit must be positive");
    }

    void http_session::run() {
        boost::asio::dispatch(
                stream_.get_executor(),
                boost::beast::bind_front_handler(
                        &http_session::do_read,
                        this->shared_from_this()));
    }

    void http_session::do_read() {
        parser_.emplace();

        parser_->body_limit(10000);

        stream_.expires_after(std::chrono::seconds(30));

        boost::beast::http::async_read(
                stream_,
                buffer_,
                *parser_,
                boost::beast::bind_front_handler(
                        &http_session::on_read,
                        shared_from_this()));
    }

    void http_session::on_read(
            boost::beast::error_code ec,
            std::size_t bytes_transferred
    ) {
        boost::ignore_unused(bytes_transferred);

        if (ec == boost::beast::http::error::end_of_stream)
            return do_close();


        // LCOV_EXCL_START
        if (ec)
            return report(ec, "read");

        // LCOV_EXCL_STOP

        if (boost::beast::websocket::is_upgrade(parser_->get())) {
            boost::make_shared<websocket_session>(
                    stream_.release_socket())->do_accept(parser_->release());
            return;
        }

        queue_write(http_kernel::handle(*doc_root_, parser_->release()));

        if (response_queue_.size() < queue_limit)
            do_read();
    }

    void http_session::queue_write(
            boost::beast::http::message_generator response
    ) {
        response_queue_.push(std::move(response));

        if (response_queue_.size() == 1)
            do_write();
    }

    void http_session::do_write() {
        if (!response_queue_.empty()) {
            bool keep_alive = response_queue_.front().keep_alive();

            boost::beast::async_write(
                    stream_,
                    std::move(response_queue_.front()),
                    boost::beast::bind_front_handler(
                            &http_session::on_write,
                            shared_from_this(),
                            keep_alive));
        }
    }

    void http_session::on_write(
            bool keep_alive,
            boost::beast::error_code ec,
            std::size_t bytes_transferred
    ) {
        boost::ignore_unused(bytes_transferred);

        // LCOV_EXCL_START

        if (ec)
            return report(ec, "write");

        // LCOV_EXCL_STOP

        if (!keep_alive) {
            return do_close();
        }

        // LCOV_EXCL_START
        if (response_queue_.size() == queue_limit)
            do_read();
        // LCOV_EXCL_STOP

        response_queue_.pop();

        do_write();
    }

    void http_session::do_close() {
        boost::beast::error_code ec;
        stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
    }


}
