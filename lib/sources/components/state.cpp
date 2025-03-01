#include <copper/components/state.hpp>

namespace copper::components {

state::state() {}

shared<state> state::instance_ = nullptr;

std::once_flag state::initialization_flag_;

shared<state> state::instance() {
  std::call_once(initialization_flag_,
                 [] { instance_ = boost::make_shared<state>(); });

  return instance_->shared_from_this();
}
}  // namespace copper::components