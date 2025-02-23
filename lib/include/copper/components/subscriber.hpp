#ifndef COPPER_COMPONENTS_SUBSCRIBER_HPP
#define COPPER_COMPONENTS_SUBSCRIBER_HPP

#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/consign.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/redis/connection.hpp>
#include <boost/redis/logger.hpp>
#include <copper/components/cache.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/state.hpp>
#include <iostream>

namespace copper::components {
containers::async_of<void> subscriber(shared<state> state) {
  boost::redis::request req;

  req.push("SUBSCRIBE", "Copper_Events");

  auto connection = co_await state->get_cache()->get_connection();

  boost::redis::generic_response resp;
  connection->set_receive_response(resp);

  while (connection->will_reconnect()) {
    co_await connection->async_exec(req, boost::redis::ignore,
                                    boost::asio::deferred);

    for (boost::system::error_code ec;;) {
      connection->receive(ec);
      if (ec == boost::redis::error::sync_receive_push_failed) {
        ec = {};

        co_await connection->async_receive(boost::asio::deferred);
      }

      if (ec) break;

      std::cout << resp.value().at(1).value << " " << resp.value().at(2).value
                << " " << resp.value().at(3).value << std::endl;

      boost::redis::consume_one(resp);
    }
  }
}
}  // namespace copper::components

#endif