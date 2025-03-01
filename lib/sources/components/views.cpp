//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/views.hpp>

namespace copper::components {
shared<views> views::instance_ = nullptr;

std::once_flag views::initialization_flag_;

shared<views> views::instance() {
  std::call_once(initialization_flag_, [] {
    instance_ = boost::make_shared<views>();
    instance_->push("404", "404");
  });

  return instance_->shared_from_this();
}
}  // namespace copper::components
