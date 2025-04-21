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

#include <boost/asio/as_tuple.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <copper/components/connection.hpp>
#include <copper/components/logger.hpp>
#include <copper/components/report.hpp>
#include <copper/components/state.hpp>
#include <iostream>

namespace copper::components {
connection::connection(boost::asio::ip::tcp::socket&& socket)
    : stream_(tcp_stream(std::move(socket))) {
  LOG("[connection@connection] scope_in");
  if (is_open()) {
    std::visit(
        [this](auto& stream) {
          auto& layer = boost::beast::get_lowest_layer(stream);
          ip_ = layer.socket().remote_endpoint().address().to_string();
          port_ = layer.socket().remote_endpoint().port();
        },
        stream_);
  }
  LOG("[connection@connection] scope_out");
}

connection::~connection() {}

// LCOV_EXCL_START
void connection::shutdown() {
  LOG("[connection@shutdown] scope_in");
  boost::system::error_code ec;

  if (auto* ws = std::get_if<websocket_stream>(&stream_)) {
    if (ws->is_open()) {
      ws->close(boost::beast::websocket::close_code::normal, ec);
      if (ec) {
        LOG("[shutdown] websocket_stream close error: " << ec.message());
      }
    }

    auto& socket = get_lowest_layer(*ws).socket();
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if (ec && ec != boost::asio::error::not_connected) {
      LOG("[shutdown] tcp_stream shutdown error (ws): " << ec.message());
    }
  } else if (auto* ts = std::get_if<tcp_stream>(&stream_)) {
    auto& socket = get_lowest_layer(*ts).socket();
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if (ec && ec != boost::asio::error::not_connected) {
      LOG("[shutdown] tcp_stream shutdown error: " << ec.message());
    }
  }
  LOG("[connection@shutdown] scope_out");
}
// LCOV_EXCL_STOP

void connection::upgrade_to_websocket(const executor_type& executor) {
  LOG("[connection@upgrade_to_websocket] scope_in");
  if (std::holds_alternative<tcp_stream>(stream_)) {
    LOG("[connection@upgrade_to_websocket] holds_alternative<tcp_stream> "
        "start");
    tcp_stream ts = std::get<tcp_stream>(std::move(stream_));
    stream_.emplace<websocket_stream>(std::move(ts));
    executor_.emplace(executor);
    LOG("[connection@upgrade_to_websocket] holds_alternative<tcp_stream> "
        "emplace");
  }
  LOG("[connection@shutdown] scope_in");
}

void connection::send(const shared<std::string const>& message) {
  LOG("[connection@send] scope_in");
  // LCOV_EXCL_START
  if (!is_websocket() || !executor_) {
    LOG("[connection@send] send ignored");
    return;
  }
  // LCOV_EXCL_STOP

  post(make_strand(*executor_),
       boost::beast::bind_front_handler(&connection::on_send,
                                        shared_from_this(), message));
  LOG("[connection@send] scope_out");
}

void connection::on_send(const shared<std::string const>& message) {
  LOG("[connection@on_send] scope_in");
  queue_.push_back(message);

  if (queue_.size() > 1) {
    LOG("[connection@on_send] already sending");
    return;
  }

  auto& ws = std::get<websocket_stream>(stream_);
  ws.async_write(boost::asio::buffer(*queue_.front()),
                 boost::beast::bind_front_handler(&connection::on_write,
                                                  shared_from_this()));
  LOG("[connection@on_send] scope_out");
}

void connection::on_write(const boost::beast::error_code& error,
                          std::size_t length) {
  LOG("[connection@on_write] scope_in");

  boost::ignore_unused(length);

  // LCOV_EXCL_START
  if (error) {
    LOG("[connection@on_write] error: " << error.what());
    return report(error, "write");
  }
  // LCOV_EXCL_STOP

  LOG("[connection@on_write] erase");
  queue_.erase(queue_.begin());

  if (!queue_.empty()) {
    LOG("[connection@on_write] non empty");
    auto& _ws = std::get<websocket_stream>(stream_);

    LOG("[connection@on_write] write dispatch");
    _ws.async_write(boost::asio::buffer(*queue_.front()),
                    boost::beast::bind_front_handler(&connection::on_write,
                                                     shared_from_this()));
  }
  LOG("[connection@on_write] scope_out");
}
}  // namespace copper::components
