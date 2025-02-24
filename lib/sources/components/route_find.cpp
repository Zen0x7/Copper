#include <copper/components/route_find.hpp>

namespace copper::components {

route_result route_find(const std::string_view &path,
                        const route &route) {
  const size_t query_ask_symbol_position = path.find('?');
  const bool path_has_parameters =
      query_ask_symbol_position != std::string::npos;
  const std::string to_be_compared{
      path_has_parameters ? path.substr(0, query_ask_symbol_position) : path};

  if (route.is_expression_) {
    const auto expression_result = route.expression_->query(to_be_compared);
    return {
        .matches_ = expression_result->matches(),
        .bindings_ = expression_result->get_bindings(),
    };
  }

  if (route.signature_ == to_be_compared)
    return {.matches_ = true, .bindings_ = {}};

  return {
      .matches_ = false,
      .bindings_ = {},
  };
}
}  // namespace copper::components