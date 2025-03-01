#ifndef COPPER_COMPONENTS_SUBSCRIBER_HPP
#define COPPER_COMPONENTS_SUBSCRIBER_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

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
 * @return async_of<void>
 */
containers::async_of<void> subscriber();
}  // namespace copper::components

#endif