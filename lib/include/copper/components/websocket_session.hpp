#ifndef COPPER_COMPONENTS_WEBSOCKET_SESSION_HPP
#define COPPER_COMPONENTS_WEBSOCKET_SESSION_HPP

#pragma once

// Copyright (C) 2025 Ian Torres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <copper/components/containers.hpp>
#include <copper/components/shared.hpp>

#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket/stream.hpp>

namespace copper::components {

/**
 * Forward core
 */
class core;

/**
 * Forward connection
 */
class connection;

/**
 * WebSocket Session
 */
class websocket_session : public shared_enabled<websocket_session> {
  shared<core> core_;
  uuid server_id_;
  shared<connection> connection_;
  boost::beast::websocket::stream<boost::beast::tcp_stream>* ws_ = nullptr;
  boost::beast::flat_buffer buffer_;

 public:
  /**
   * Constructor
   *
   * @param core
   * @param server_id
   * @param connection
   */
  explicit websocket_session(const shared<core>& core,
                             const uuid& server_id,
                             const shared<connection>& connection);

  /**
   * Destructor
   */
  ~websocket_session();

  /**
   * Start
   */
  void start();

  /**
   * On accept
   *
   * @param error
   */
  void on_accept(boost::system::error_code error);

  /**
   * Do read
   */
  void do_read();

  /**
   * On read
   * @param error
   * @param bytes_transferred
   */
  void on_read(boost::system::error_code error, std::size_t bytes_transferred);
};

}  // namespace copper::components

#endif