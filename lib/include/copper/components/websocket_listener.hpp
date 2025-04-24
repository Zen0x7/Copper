#ifndef COPPER_COMPONENTS_WEBSOCKET_LISTENER_HPP
#define COPPER_COMPONENTS_WEBSOCKET_LISTENER_HPP

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

#include <boost/asio/ip/tcp.hpp>
#include <copper/components/containers.hpp>

namespace copper::components {

/**
 * Forward core
 */
class core;

/**
 * Websocket Listener
 */
class websocket_listener : public shared_enabled<websocket_listener> {
 public:
  /**
   * Constructor
   *
   * @param io_context
   * @param core
   * @param server_id
   * @param endpoint
   */
  websocket_listener(boost::asio::io_context& io_context,
                     const shared<core>& core,
                     const uuid& server_id,
                     const boost::asio::ip::tcp::endpoint& endpoint);

  /**
   * Run
   */
  void run();

 private:
  /**
   * Do accept
   */
  void do_accept();

  /**
   * Server ID
   */
  uuid server_id_;

  /**
   * Core
   */
  shared<core> core_;

  /**
   * IO Context
   */
  boost::asio::io_context& ioc_;

  /**
   * Acceptor
   */
  boost::asio::ip::tcp::acceptor acceptor_;
};

}  // namespace copper::components

#endif
