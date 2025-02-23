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
/**
 * Subscriber
 *
 * @param state
 * @return async_of<void>
 */
containers::async_of<void> subscriber(shared<state> state);
}  // namespace copper::components

#endif