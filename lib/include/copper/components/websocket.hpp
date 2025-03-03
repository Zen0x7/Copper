#ifndef COPPER_COMPONENTS_WEBSOCKET_HPP
#define COPPER_COMPONENTS_WEBSOCKET_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/uuid/random_generator.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/uuid.hpp>

namespace copper::components {
/**
 * Websocket
 */
class websocket : public shared<websocket> {
 public:
  /**
   * ID
   */
  uuid id_ = boost::uuids::random_generator()();

  /**
   * Session ID
   */
  uuid session_id_;

  /**
   * Stream
   */
  boost::beast::websocket::stream<boost::beast::tcp_stream> stream_;

  /**
   * Constructor
   *
   * @param session_id
   * @param socket
   */
  explicit websocket(uuid session_id, boost::asio::ip::tcp::socket&& socket);

  /**
   * Destructor
   */
  ~websocket();
};
}  // namespace copper::components

#endif