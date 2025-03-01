//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/route_find.hpp>

namespace copper::components {

route_result route_find(const std::string &url, const route &route) {
  if (route.is_expression_) {
    const auto _result = route.expression_->query(url);
    return {
        .matches_ = _result->matches(),
        .bindings_ = _result->get_bindings(),
    };
  }

  if (route.signature_ == url) return {.matches_ = true, .bindings_ = {}};

  return {
      .matches_ = false,
      .bindings_ = {},
  };
}
}  // namespace copper::components