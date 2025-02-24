#ifndef COPPER_COMPONENTS_DETECT_SESSION_HPP
#define COPPER_COMPONENTS_DETECT_SESSION_HPP

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
 * @return async_of<void>
 */
containers::async_of<void> detect_session(
    shared<state> state, uuid server_id, uuid session_id,
    typename boost::beast::tcp_stream::rebind_executor<
        boost::asio::strand<boost::asio::io_context::executor_type> >::other
        stream,
    boost::asio::ssl::context &ctx, boost::beast::string_view doc_root);

}  // namespace copper::components

#endif