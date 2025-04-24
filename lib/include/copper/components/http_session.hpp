#ifndef COPPER_COMPONENTS_HTTP_SESSION_HPP
#define COPPER_COMPONENTS_HTTP_SESSION_HPP

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
#include <boost/beast/http/message_generator.hpp>
#include <boost/beast/http/string_body.hpp>

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
 * HTTP Session
 */
class http_session : public shared_enabled<http_session> {
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

  /**
   * Request
   */
  boost::beast::http::request<boost::beast::http::string_body> req_;

  /**
   * Buffer
   */
  boost::beast::flat_buffer buffer_;

 public:
  /**
   * Constructor
   *
   * @param core
   * @param server_id
   * @param connection
   */
  explicit http_session(const shared<core>& core,
                        const uuid& server_id,
                        const shared<connection>& connection);

  /**
   * Destructor
   */
  ~http_session();

  /**
   * Start
   *
   * @return void
   */
  void start();

  /**
   * Do read
   *
   * @return void
   */
  void do_read();

  /**
   * On read
   *
   * @param error
   * @param bytes_transferred
   * @return void
   */
  void on_read(boost::beast::error_code error, std::size_t bytes_transferred);

  /**
   * Send response
   *
   * @param msg
   */
  void send_response(boost::beast::http::message_generator&& msg);

  /**
   * On write
   *
   * @param keep_alive
   * @param ec
   * @param bytes_transferred
   * @return void
   */
  void on_write(bool keep_alive,
                boost::beast::error_code ec,
                std::size_t bytes_transferred);

  /**
   * Do close
   *
   * @return void
   */
  void do_close() const;
};

}  // namespace copper::components

#endif
