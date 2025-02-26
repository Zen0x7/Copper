#include <copper/components/route_find.hpp>

namespace copper::components {

route_result route_find(const std::string &url, const route &route) {
  if (route.is_expression_) {
    const auto expression_result = route.expression_->query(url);
    return {
        .matches_ = expression_result->matches(),
        .bindings_ = expression_result->get_bindings(),
    };
  }

  if (route.signature_ == url) return {.matches_ = true, .bindings_ = {}};

  return {
      .matches_ = false,
      .bindings_ = {},
  };
}
}  // namespace copper::components