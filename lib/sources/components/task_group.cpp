//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/task_group.hpp>

namespace copper::components {

// LCOV_EXCL_START: The only one enabled to run this method is signal_handler,
// then, tests requires a graceful shutdown SIGINT.
void task_group::emit(const boost::asio::cancellation_type type) {
  auto _guard = std::scoped_lock{mutex_};
  for (auto &_signal : signals_) _signal.emit(type);
}
// LCOV_EXCL_STOP
}  // namespace copper::components