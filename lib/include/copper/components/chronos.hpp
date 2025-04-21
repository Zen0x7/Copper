#ifndef COPPER_COMPONENTS_CHRONOS_HPP
#define COPPER_COMPONENTS_CHRONOS_HPP

#pragma once

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

#include <chrono>

namespace copper::components::chronos {

/**
 * Retrieves current UNIX timestamp in nanoseconds
 *
 * @return long
 */
inline long now() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

/**
 * Converts clock time point to UNIX timestamp in
 * nanoseconds
 *
 * @param clock
 * @return long
 */
inline long to_timestamp(std::chrono::system_clock::time_point clock) {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             clock.time_since_epoch())
      .count();
}

}  // namespace copper::components::chronos

#endif
