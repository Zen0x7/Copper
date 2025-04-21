#ifndef COPPER_COMPONENTS_CONNECTION_HPP
#define COPPER_COMPONENTS_CONNECTION_HPP

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
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/uuid/random_generator.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/uuid.hpp>
#include <deque>

namespace copper::components {
/**
 * Connection
 */
class connection : public shared_enabled<connection> {
 public:
  /**
   * TCP stream alias
   */
  using tcp_stream = boost::beast::tcp_stream;

  /**
   * Websocket stream alias
   */
  using websocket_stream = boost::beast::websocket::stream<tcp_stream>;

  /**
   * Executor type alias
   */
  using executor_type = boost::asio::any_io_executor;

  /**
   * ID
   */
  uuid id_ = boost::uuids::random_generator()();

  /**
   * IP
   */
  std::string ip_;

  /**
   * Port
   */
  uint_least16_t port_;

  /**
   * Stream
   */
  std::variant<tcp_stream, websocket_stream> stream_;

  /**
   * Executor
   */
  std::optional<executor_type> executor_;

  /**
   * Queue
   */
  std::deque<shared<std::string const>> queue_;

  /**
   * Mutex
   */
  std::mutex mutex_;

  /**
   * Writing
   */
  bool writing_;

  /**
   * Constructor
   *
   * @param socket
   */
  explicit connection(boost::asio::ip::tcp::socket&& socket);

  /**
   * Destructor
   */
  ~connection();

  /**
   * Is open?
   *
   * @return bool
   */
  bool is_open() const {
    return std::visit(
        [](auto& stream) {
          return boost::beast::get_lowest_layer(stream).socket().is_open();
        },
        stream_);
  }

  /**
   * Is websocket?
   * @return bool
   */
  bool is_websocket() const {
    return std::holds_alternative<websocket_stream>(stream_);
  }

  /**
   * Graceful shutdown
   */
  void shutdown();

  /**
   * Upgrade to websocket
   */
  void upgrade_to_websocket(const executor_type& executor);

  /**
   * Send
   *
   * @param message
   */
  void send(const shared<std::string const>& message);

 private:
  /**
   * On send
   *
   * @param message
   */
  void on_send(const shared<std::string const>& message);

  /**
   * On write
   *
   * @param error
   */
  void on_write(const boost::beast::error_code& error, std::size_t);
};
}  // namespace copper::components

#endif
