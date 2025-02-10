#pragma once


#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/scope/scope_exit.hpp>

#include <copper/components/http_session.hpp>

using executor_type = boost::asio::strand<boost::asio::io_context::executor_type>;
using stream_type   = typename boost::beast::tcp_stream::rebind_executor<boost::asio::strand<boost::asio::io_context::executor_type>>::other;
using acceptor_type = typename boost::asio::ip::tcp::acceptor::rebind_executor<executor_type>::other;

namespace copper::components {
    boost::asio::awaitable<void, boost::asio::strand<boost::asio::io_context::executor_type>>
    detect_session(
            stream_type stream,
            boost::asio::ssl::context& ctx,
            boost::beast::string_view doc_root);
}