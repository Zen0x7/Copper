#include <copper/components/http_route_match.hpp>

namespace copper::components {

http_route_result http_route_match(http_method method,
                                   const std::string_view &path,
                                   const http_route &route) {
  const size_t query_ask_mark_position = path.find('?');
  const bool path_has_params = query_ask_mark_position != std::string::npos;
  const std::string to_compare{
      path_has_params ? path.substr(0, query_ask_mark_position) : path};
  const bool method_rejection = method != route.method_;
  const bool expression_rejection = !route.is_expression_;
  const bool signature_rejection = to_compare != route.signature_;

  if (expression_rejection && (method_rejection || signature_rejection)) {
    return {.matches_ = false, .bindings_ = {}};
  } else if (!expression_rejection) {
    const std::string query{path};
    const auto expression_result_ = route.expression_->query(query);

    return {
        .matches_ = expression_result_->matches(),
        .bindings_ = expression_result_->get_bindings(),
    };
  } else {
    return {.matches_ = true, .bindings_ = {}};
  }
}
}  // namespace copper::components