#ifndef COPPER_COMPONENTS_DETECT_SESSION_HPP
#define COPPER_COMPONENTS_DETECT_SESSION_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/session_handler.hpp>

namespace copper::components {

/**
 * Detect session protocol
 *
 * @param stream
 * @param ctx
 * @param doc_root
 * @return async_of<void>
 */
containers::async_of<void> protocol_handler(uuid server_id, uuid session_id,
                                            boost::beast::tcp_stream stream,
                                            boost::beast::string_view doc_root);

}  // namespace copper::components

#endif