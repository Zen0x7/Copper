#pragma once

#include <copper/components/http_session.hpp>
#include <copper/models/session.hpp>

namespace copper::components {

/**
 * Detect session protocol
 *
 * @param stream
 * @param ctx
 * @param doc_root
 * @return boost::asio::awaitable<T, U> Callback
 */
boost::asio::awaitable<
    void, boost::asio::strand<boost::asio::io_context::executor_type> >
detect_session(
    shared<state> state, shared<copper::models::session> session,
    typename boost::beast::tcp_stream::rebind_executor<
        boost::asio::strand<boost::asio::io_context::executor_type> >::other
        stream,
    boost::asio::ssl::context &ctx, boost::beast::string_view doc_root);

}  // namespace copper::components