#ifndef COPPER_COMPONENTS_DETECT_SESSION_HPP
#define COPPER_COMPONENTS_DETECT_SESSION_HPP

#pragma once

#include <copper/components/session_handler.hpp>
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
containers::async_of<void> protocol_handler(shared<state> state, uuid server_id,
                                            uuid session_id,
                                            boost::beast::tcp_stream stream,
                                            boost::beast::string_view doc_root);

}  // namespace copper::components

#endif