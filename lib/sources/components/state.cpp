#include <copper/components/cache.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/database.hpp>
#include <copper/components/logger.hpp>
#include <copper/components/router.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/state.hpp>
#include <copper/components/views.hpp>

namespace copper::components {

state::state() { views::instance()->push("404", "404"); }

shared<state> state::instance_ = nullptr;

std::once_flag state::initialization_flag_;

shared<state> state::instance() {
  std::call_once(initialization_flag_,
                 [] { instance_ = boost::make_shared<state>(); });

  return instance_->shared_from_this();
}
}  // namespace copper::components