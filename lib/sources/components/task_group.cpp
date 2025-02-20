#include <copper/components/task_group.hpp>

namespace copper::components {

// LCOV_EXCL_START: The only one enabled to run this method is signal_handler,
// then, tests requires a graceful shutdown SIGINT.
void task_group::emit(boost::asio::cancellation_type type) {
  auto guard = std::lock_guard{mutex_};
  for (auto &signal : signals_) signal.emit(type);
}
// LCOV_EXCL_STOP
}  // namespace copper::components