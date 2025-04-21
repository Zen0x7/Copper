#ifndef COPPER_COMPONENTS_SIGNAL_HANDLER_HPP
#define COPPER_COMPONENTS_SIGNAL_HANDLER_HPP

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

#include <copper/components/core.hpp>
#include <functional>

// LCOV_EXCL_START
namespace copper::components {
inline std::atomic_bool signal_handler_shutting_down_ = false;
inline std::function<void(int)> signal_handler_callback_;

/**
 * Signal handler trampoline
 *
 * @param sig
 */
inline void signal_handler_trampoline(int sig) {
  if (signal_handler_shutting_down_.exchange(true))
    return;

  std::puts("SIGNAL received ... ");

  if (signal_handler_callback_)
    signal_handler_callback_(sig);
}

/**
 * Signal handler
 * @param callback
 */
inline void signal_handler(callback_of<int> callback) {
  signal_handler_callback_ = std::move(callback);

  ::signal(SIGINT, signal_handler_trampoline);
  ::signal(SIGTERM, signal_handler_trampoline);

  std::puts("SIGNAL setup done ...");
}
}  // namespace copper::components
// LCOV_EXCL_STOP

#endif
