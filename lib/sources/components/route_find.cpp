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