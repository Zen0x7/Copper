//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/state.hpp>

namespace copper::components {

// LCOV_EXCL_START
state::state() {}

shared<state> state::instance_ = nullptr;

std::once_flag state::initialization_flag_;

shared<state> state::instance() {
  std::call_once(initialization_flag_,
                 [] { instance_ = boost::make_shared<state>(); });

  return instance_->shared_from_this();
}
// LCOV_EXCL_STOP

}  // namespace copper::components