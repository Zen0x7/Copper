#include <copper/components/task_group.hpp>

namespace copper::components {

// LCOV_EXCL_START: The only one enabled to run this method is signal_handler,
// then, tests requires a graceful shutdown SIGINT.
void task_group::emit(const boost::asio::cancellation_type type) {
  auto _guard = std::lock_guard{mutex_};
  for (auto &_signal : signals_) _signal.emit(type);
}
// LCOV_EXCL_STOP
}  // namespace copper::components