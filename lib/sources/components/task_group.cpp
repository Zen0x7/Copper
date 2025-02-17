#include <copper/components/task_group.hpp>

namespace copper::components {

    void task_group::emit(boost::asio::cancellation_type type) {
      auto guard = std::lock_guard{mutex_};
      for (auto &signal: signals_)
        signal.emit(type);
    }
}