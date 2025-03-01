#ifndef COPPER_COMPONENTS_WEBSOCKET_HANDLER_HPP
#define COPPER_COMPONENTS_WEBSOCKET_HANDLER_HPP

#pragma once

#include <boost/asio/as_tuple.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/detect_ssl.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/scope/scope_exit.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/report.hpp>
#include <copper/components/request.hpp>
#include <copper/components/state.hpp>
#include <copper/components/uuid.hpp>

namespace copper::components {

/**
 * Run websocket session
 *
 * @tparam Stream
 * @param stream
 * @param buffer
 * @param req
 * @return
 */
containers::async_of<void> websocket_handler(
    uuid /* server_id */, uuid /* session_id */,
    boost::beast::tcp_stream &stream, boost::beast::flat_buffer &buffer,
    request req, boost::beast::string_view /*doc_root*/);

}  // namespace copper::components

#endif