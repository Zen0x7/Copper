#ifndef COPPER_COMPONENTS_SESSION_HANDLER_HPP
#define COPPER_COMPONENTS_SESSION_HANDLER_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/containers.hpp>
#include <copper/components/websocket_handler.hpp>
#include <copper/models/request.hpp>

namespace copper::components {

/**
 * Run http session
 *
 * @tparam Stream
 * @param stream
 * @param buffer
 * @param doc_root
 * @return
 */
containers::async_of<void> session_handler(uuid server_id, uuid session_id,
                                           boost::beast::tcp_stream &stream,
                                           boost::beast::flat_buffer &buffer,
                                           boost::beast::string_view doc_root);

}  // namespace copper::components

#endif