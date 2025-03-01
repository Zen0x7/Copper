//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/chronos.hpp>

namespace copper::components::chronos {
long now() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

long to_timestamp(const std::chrono::system_clock::time_point clock) {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             clock.time_since_epoch())
      .count();
}
}  // namespace copper::components::chronos