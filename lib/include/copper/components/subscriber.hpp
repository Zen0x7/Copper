#ifndef COPPER_COMPONENTS_SUBSCRIBER_HPP
#define COPPER_COMPONENTS_SUBSCRIBER_HPP

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

#include <boost/asio/awaitable.hpp>
#include <copper/components/containers.hpp>

namespace copper::components {

/**
 * Forward core
 */
class core;

/**
 * Subscriber
 */
class subscriber : public shared_enabled<subscriber> {
  /**
   * Core
   */
  shared<core> core_;

  /**
   * Server ID
   */
  uuid server_id_;

  /**
   * Connection
   */
  shared<boost::redis::connection> connection_;

  /**
   * Request
   */
  boost::redis::request request_;

  /**
   * Response
   */
  boost::redis::generic_response response_;

  /**
   * Executor
   */
  boost::asio::any_io_executor executor_;

 public:
  /**
   * Start
   *
   * @param core
   * @param server_id
   */
  static void start(const shared<core>& core, const uuid& server_id);

  /**
   * Constructor
   *
   * @param core
   * @param server_id
   * @param connection
   */
  subscriber(const shared<core>& core,
             uuid server_id,
             const shared<boost::redis::connection>& connection);

  /**
   * Run
   */
  void run();

  /**
   * On subscribe
   *
   * @param error
   */
  void on_subscribe(boost::system::error_code error);

  /**
   * Do receive
   */
  void do_receive();

  /**
   * On receive
   *
   * @param error
   */
  void on_receive(boost::system::error_code error);

 private:
  /**
   * Error
   */
  boost::system::error_code error_;
};
}  // namespace copper::components

#endif
