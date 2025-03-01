#ifndef COPPER_COMPONENTS_SUBSCRIBER_HPP
#define COPPER_COMPONENTS_SUBSCRIBER_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/asio/awaitable.hpp>
#include <copper/components/containers.hpp>

namespace copper::components {

/**
 * Subscriber
 *
 * @return async_of<void>
 */
containers::async_of<void> subscriber();
}  // namespace copper::components

#endif