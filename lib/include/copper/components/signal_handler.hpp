#pragma once

#include <copper/components/detect_session.hpp>
#include <copper/components/task_group.hpp>
#include <copper/components/containers.hpp>

namespace copper::components {

/**
 * Signal handler
 *
 * @param task_group
 * @return
 */
    containers::async_of<void>
    signal_handler(shared<task_group> task_group);

}  // namespace copper::components