#include <copper/components/views.hpp>

namespace copper::components {
shared<views> views::instance_ = nullptr;

std::once_flag views::initialization_flag_;

shared<views> views::instance() {
  std::call_once(initialization_flag_,
                 [] { instance_ = boost::make_shared<views>(); });

  return instance_->shared_from_this();
}
}  // namespace copper::components
