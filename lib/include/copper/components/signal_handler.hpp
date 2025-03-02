#ifndef COPPER_COMPONENTS_SIGNAL_HANDLER_HPP
#define COPPER_COMPONENTS_SIGNAL_HANDLER_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/containers.hpp>
#include <copper/components/protocol_handler.hpp>
#include <copper/components/task_group.hpp>

namespace copper::components {

/**
 * Signal handler
 *
 * @param task_group
 * @return async_of<void>
 */
containers::async_of<void> signal_handler(shared<task_group> task_group);

}  // namespace copper::components

#endif