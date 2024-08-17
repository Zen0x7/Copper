#pragma once

#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

class session : public std::enable_shared_from_this<session> {
public:
    session(boost::asio::ip::tcp::socket socket) : socket_(std::move(socket)) { }

    void start() {
        do_read();
    }

private:
    void do_read();

    void do_write(std::size_t length);

    boost::asio::ip::tcp::socket socket_;

    enum { max_length = 1024 };

    char data_[max_length];
};