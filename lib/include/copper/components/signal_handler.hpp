#ifndef COPPER_COMPONENTS_SIGNAL_HANDLER_HPP
#define COPPER_COMPONENTS_SIGNAL_HANDLER_HPP

#pragma once

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
containers::async_of<void> signal_handler(const shared<task_group>& task_group);

}  // namespace copper::components

#endif