#ifndef COPPER_COMPONENTS_TCP_SESSION_HPP
#define COPPER_COMPONENTS_TCP_SESSION_HPP

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

#include <boost/beast/core/tcp_stream.hpp>
#include <copper/components/shared.hpp>

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
 * TCP Session
 */
class tcp_session : public shared_enabled<tcp_session> {
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
  shared<connection> connection_;

  /**
   * Stream
   */
  boost::beast::tcp_stream* stream_ = nullptr;

  /**
   * Socket
   */
  boost::beast::basic_stream<boost::asio::ip::tcp>* socket_ = nullptr;

 public:
  /**
   * Constructor
   *
   * @param core
   * @param server_id
   * @param connection
   */
  explicit tcp_session(const shared<core>& core,
                       const uuid& server_id,
                       const shared<connection>& connection);

  /**
   * Destructor
   */
  ~tcp_session();

  /**
   * Start
   */
  void start() { do_welcome(); }

 private:
  /**
   * Do welcome
   */
  void do_welcome();

  /**
   * Do welcome ack
   *
   * @param closes
   */
  void do_welcome_ack(bool closes);

  /**
   * Do read
   */
  void do_read();

  /**
   * Enum
   */
  enum { max_length = 2048 };

  /**
   * Data
   */
  char data_[max_length]{};
};

}  // namespace copper::components

#endif
