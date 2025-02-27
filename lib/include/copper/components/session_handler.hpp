#ifndef COPPER_COMPONENTS_SESSION_HANDLER_HPP
#define COPPER_COMPONENTS_SESSION_HANDLER_HPP

#pragma once

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/uuid/random_generator.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/database.hpp>
#include <copper/components/header.hpp>
#include <copper/components/kernel.hpp>
#include <copper/components/logger.hpp>
#include <copper/components/report.hpp>
#include <copper/components/state.hpp>
#include <copper/components/websocket_handler.hpp>
#include <copper/models/request.hpp>
#include <copper/models/response.hpp>
#include <copper/models/session.hpp>

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
containers::async_of<void> session_handler(shared<state> state, uuid server_id,
                                           uuid session_id,
                                           boost::beast::tcp_stream &stream,
                                           boost::beast::flat_buffer &buffer,
                                           boost::beast::string_view doc_root);

}  // namespace copper::components

#endif