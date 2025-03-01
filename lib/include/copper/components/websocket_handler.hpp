#ifndef COPPER_COMPONENTS_WEBSOCKET_HANDLER_HPP
#define COPPER_COMPONENTS_WEBSOCKET_HANDLER_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/request.hpp>
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