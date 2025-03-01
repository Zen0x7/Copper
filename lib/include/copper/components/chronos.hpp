#ifndef COPPER_COMPONENTS_CHRONOS_HPP
#define COPPER_COMPONENTS_CHRONOS_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <chrono>

namespace copper::components::chronos {

/**
 * Retrieves current UNIX timestamp in nanoseconds
 *
 * @return long
 */
long now();

/**
 * Converts clock time point to UNIX timestamp in nanoseconds
 *
 * @param clock
 * @return long
 */
long to_timestamp(std::chrono::system_clock::time_point clock);

}  // namespace copper::components::chronos

#endif