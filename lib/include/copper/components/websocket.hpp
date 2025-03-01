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
#include <copper/components/shared.hpp>

namespace copper::components {
class websocket : public shared<websocket> {
 public:
  boost::beast::websocket::stream<boost::beast::tcp_stream> stream_;
  websocket(boost::asio::ip::tcp::socket&& socket)
      : stream_(std::move(socket)) {}
};
}  // namespace copper::components

#endif